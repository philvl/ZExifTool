/*
MIT License

Copyright (c) 2020 Philippe Vianney-Liaud | https://github.com/philvl

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "ZExifToolProcess.h"

#include <QElapsedTimer>
#include <QMutex>
#include <QDebug>

// Init static variables (DO NOT EDIT)
const quint32   ZExifToolProcess::CMD_ID_MIN=          1;
const quint32   ZExifToolProcess::CMD_ID_MAX= 4000000000;
const qsizetype ZExifToolProcess::CMD_ID_LEN= QByteArray::number(CMD_ID_MAX).length();
      quint32   ZExifToolProcess::_nextCmdId= CMD_ID_MIN;

/**
 * Constructs a ZExifToolProcess object with exiftool executable path, perl executable path and the given parent.
 * Internally, set the startup arguments of the exiftool process ("-stay_open true -@ -").
 * Each new ZExifToolProcess object maintains an independent exiftool process/application.
 */
ZExifToolProcess::ZExifToolProcess(const QString &etExePath, const QString &perlExePath, QObject *parent) : QObject{parent} {
    // Setup execTimer
    _execTimer= new QElapsedTimer();

    // Setup process
    _process= new QProcess();
    _process->setProcessChannelMode(QProcess::SeparateChannels);
    //-- Connect signal to signal
    connect(_process, &QProcess::started,      this, &ZExifToolProcess::started);
    connect(_process, &QProcess::stateChanged, this, &ZExifToolProcess::stateChanged);
    //-- Connect signal to internal slot
    connect(_process, &QProcess::readyReadStandardOutput, this, [=] { readOutput(QProcess::StandardOutput); });
    connect(_process, &QProcess::readyReadStandardError,  this, [=] { readOutput(QProcess::StandardError);  });
    connect(_process, &QProcess::errorOccurred,           this, &ZExifToolProcess::onErrorOccurred);
    connect(_process, &QProcess::finished,                this, &ZExifToolProcess::onFinished);

    // Setup internal variables
    _processError=     QProcess::UnknownError;
    _pendingTerminate= false;
    _cmdRunning=       false;

    // Prepare exiftool startup command
    QString program= etExePath;
    QStringList arguments;
    if(!perlExePath.isEmpty()) {
        program= perlExePath;
        arguments << etExePath;
    }
    arguments << QLatin1String("-stay_open"); arguments << QLatin1String("true");
    arguments << QLatin1String("-@");         arguments << QLatin1String("-");

    _process->setProgram(program);
    _process->setArguments(arguments);
}

/**
 * Destructs the ZExifToolProcess object, i.e., killing exiftool process immediately.
 * WARNING: This function may corrupt files if exiftool performs write operations.
 */
ZExifToolProcess::~ZExifToolProcess() {
    delete _process;
    delete _execTimer;
}



/**
 * Start the exiftool process
 */
void ZExifToolProcess::start() {
    if(_process->state() != QProcess::NotRunning) {
        qWarning("ZExifToolProcess::start: ExifTool is already running");
        return;
    }

    // Clear errors
    _processError= QProcess::UnknownError;
    _errorString=  QString();

    // Start exiftool process
    _process->start(QProcess::ReadWrite);
}

/**
 * Attempts to terminate the exiftool process gracefully.
 * Flush the command queue, then instructs exiftool to terminate after the current command is completed.
 * After calling this function, sending commands to exiftool becomes impossible.
 * This function returns immediately. If you want to wait for the exiftool
 * process to finish, use waitForFinished() after terminate().
 */
void ZExifToolProcess::terminate() {
    // If exiftool process running, send close command
    if(_process->state() == QProcess::Running) {
        _cmdQueue.clear();
        _pendingTerminate= true;
        _process->write("-stay_open\nfalse\n");
        _process->closeWriteChannel();
        return;
    }
    // Otherwise, close exiftool using OS system call (WM_CLOSE [Windows] or SIGTERM [Unix])
    _process->terminate();
}

/**
 * Terminate the exiftool process gracefully.
 * Flush the command queue, then blocks until the current command is completed, the exiftool process has finished
 * and the finished() signal has been emitted, or until msecs milliseconds have passed (Default: no timeout).
 * Calling this function from the main (GUI) thread might cause your user interface to freeze.
 */
void ZExifToolProcess::terminateSafely(int msecs) {
    terminate();
    if(_process->state() != QProcess::NotRunning)
        if(!_process->waitForFinished(msecs))
            _process->kill();
}

/**
 * Kills the exiftool process, causing it to exit immediately.
 * WARNING: This function may corrupt files if exiftool performs write operations.
 */
void ZExifToolProcess::kill() {
    _process->kill();
}



/**
 * Returns true if the exiftool process is running
 */
bool ZExifToolProcess::isRunning() const {
    return _process->state() == QProcess::Running;
}

/**
 * Returns true if the exiftool process is busy, i.e., a command is running
 */
bool ZExifToolProcess::isBusy() const {
    return _cmdRunning;
}

/**
 * Returns the command line arguments the exiftool process start with.
 */
QStringList ZExifToolProcess::arguments() const {
    return _process->arguments();
}

/**
 * Returns the type of error that occurred last (FailedToStart, Crashed, Timedout, ReadError, WriteError or UnknownError).
 */
QProcess::ProcessError ZExifToolProcess::error() const {
    return _processError;
}

/**
 * Returns a human-readable description of the last error that occurred.
 */
QString ZExifToolProcess::errorString() const {
    return _errorString;
}

/**
 * Returns the exit status of the last exiftool process that finished (NormalExit | CrashExit).
 * On Windows, if the process was terminated with TerminateProcess() from another application, this function will still return NormalExit unless the exit code is less than 0
 */
QProcess::ExitStatus ZExifToolProcess::exitStatus() const {
    return _process->exitStatus();
}

/**
 * Returns the exit code of the last exiftool process that finished.
 * This value is not valid unless exitStatus() returns NormalExit.
 */
int ZExifToolProcess::exitCode() const {
    return _process->exitCode();
}

/**
 * Returns the native process identifier for the running exiftool process, if available. If no process is currently running, 0 is returned.
 */
qint64 ZExifToolProcess::processId() const {
    return _process->processId();
}

/**
 * Returns the current state of exiftool process (NotRunning, Starting or Running).
 */
QProcess::ProcessState ZExifToolProcess::state() const {
    return _process->state();
}

/**
 * Blocks until exiftool process has started and the started() signal has been emitted,
 * or until msecs milliseconds have passed (Default: 1 sec timeout).
 */
bool ZExifToolProcess::waitForStarted(int msecs) {
    return _process->waitForStarted(msecs);
}

/**
 * Blocks until exiftool process has finished and the finished() signal has been emitted,
 * or until msecs milliseconds have passed (Default: no timeout).
 */
bool ZExifToolProcess::waitForFinished(int msecs) {
    return _process->waitForFinished(msecs);
}



/**
 * Sends the command to exiftool and returns a unique command ID.
 * This function is thread-safe, command ID is unique even in threaded environment
 * Return 0 on errors (ExitTool not running, or args is empty)
 */
quint32 ZExifToolProcess::command(const QByteArrayList &args) {
    if(_process->state() != QProcess::Running || _pendingTerminate || args.isEmpty())
        return 0;

    // ThreadSafe incrementation of _nextCmdId
    static QMutex mutex; mutex.lock();
    quint32 cmdId= _nextCmdId;
    if(_nextCmdId++ >= CMD_ID_MAX)
        _nextCmdId= CMD_ID_MIN;
    mutex.unlock();

    // String representation of _cmdId with leading zeros to obtain a constant size
    // corresponding to the number of digits required for CMD_ID_MAX
    QByteArray cmdIdStr= QByteArray::number(cmdId).rightJustified(CMD_ID_LEN, '0');

    // Build command string from args
    QByteArray cmdStr;
  #ifdef Q_OS_WINDOWS
    //-- Input/output text formatting
    cmdStr.append("-charset\nfilename=utf8\n"); // Force UTF‑8 encoding for the argument file under MS Windows (ExitTool FAQ 18)
  #endif
    cmdStr+= args.join('\n') + '\n';
    cmdStr+= "-echo4\n{ready"+cmdIdStr+"}\n";   // Echo {ready###} to stderr after processing is complete
    cmdStr+= "-execute"+cmdIdStr+"\n";          // Execute command and echo {ready###} to stdout after processing is complete

    _cmdQueue << cmdStr;
    execNextCmd();
    return cmdId;
}

void ZExifToolProcess::execNextCmd() {
    if(_process->state() != QProcess::Running || _pendingTerminate || _cmdRunning)
        return;

    // Clear stdOut and stdErr buffers
    _process->readAllStandardOutput();
    _process->readAllStandardError();
    _readyBeginPos[0]= _readyBeginPos[1]= -1;
    _readyEndPos[0]=   _readyEndPos[1]=   -1;
    _outBuff[0]=       _outBuff[1]=    QByteArray();

    // Exec next exiftool command if queue is not empty
    if(!_cmdQueue.isEmpty()) {
        _cmdRunning= true;
        _execTimer->start();
        _process->write(_cmdQueue.takeFirst());
    }
}



void ZExifToolProcess::readOutput(const QProcess::ProcessChannel channel) {
    _process->setReadChannel(channel); // Sets the current read channel of the QProcess
    const qsizetype bytesAvailable= _process->bytesAvailable();

    // Append new data to the buffer
    _outBuff[channel]+= _process->readAll();
    emit newBuffData(channel, _outBuff[channel].last(bytesAvailable)); // For debug only: emit signal which contains the new data without any treatment

    // Response should end with '{ready###}\n' or '{ready###}\r\n'.
    // The '###' is a constant size string representation of cmdID, i.e., CMD_ID_LEN.
    //-- Looking for "{ready"
    if(_readyBeginPos[channel] < 0) {
        //-- Continue search of "{ready" where we left off
        qsizetype searchPos= _outBuff[channel].size() - bytesAvailable - 5;
        if(searchPos < 0)
            searchPos= 0;

        _readyBeginPos[channel]= _outBuff[channel].indexOf("{ready", searchPos);
        if(_readyBeginPos[channel] < 0)
            return;
    }
    //-- Wait for complete response (must end with '}\n' or '}\r\n')
    if(_readyEndPos[channel] < 0) {
        qsizetype endBracketPos= _readyBeginPos[channel] + 5 + CMD_ID_LEN + 1;
        if(_outBuff[channel].size() > endBracketPos + 1 && _outBuff[channel].sliced(endBracketPos, 2) == "}\n")
            _readyEndPos[channel]= endBracketPos + 1;
        else if(_outBuff[channel].size() > endBracketPos + 2 && _outBuff[channel].sliced(endBracketPos, 3) == "}\r\n")
            _readyEndPos[channel]= endBracketPos + 2;
        else return;
    }

    // Check if the standardOutput and standardError channels have a complete response
    if(_readyEndPos[QProcess::StandardOutput] < 0 || _readyEndPos[QProcess::StandardError] < 0)
        return;

    // Extract cmdId and check coherence
    quint32 cmdIdStdOut= _outBuff[QProcess::StandardOutput].sliced(_readyBeginPos[QProcess::StandardOutput] + 6, CMD_ID_LEN).toUInt();
    quint32 cmdIdStdErr= _outBuff[QProcess::StandardError].sliced( _readyBeginPos[QProcess::StandardError]  + 6, CMD_ID_LEN).toUInt();
    if(!cmdIdStdOut || !cmdIdStdErr || cmdIdStdOut != cmdIdStdErr) {
        qCritical() << "ZExifToolProcess::onReadyRead: ERROR: StdOut and StdErr is not sync: cmdIdStdOut and cmdIdStdErr does not match, or conversion failed from {ready###} (i.e., null value)";
        qCritical() << "    cmdIdStdOut:" << cmdIdStdOut << " stdOutString:" << _outBuff[QProcess::StandardOutput].sliced(_readyBeginPos[QProcess::StandardOutput], CMD_ID_LEN + 7);
        qCritical() << "    cmdIdStdErr:" << cmdIdStdErr << " stdErrString:" << _outBuff[QProcess::StandardError].sliced( _readyBeginPos[QProcess::StandardError],  CMD_ID_LEN + 7);
        setProcessErrorAndEmit(QProcess::ReadError, "StdOut and StdErr is not sync: cmdIdStdOut and cmdIdStdErr does not match, or conversion failed from {ready###} (i.e., null value)");
        terminate(); return;
    }
    else {
        // Extract stdOut and stdErr data from buffers
        const QByteArray stdOut= _outBuff[QProcess::StandardOutput].first(_readyBeginPos[QProcess::StandardOutput]);
        const QByteArray stdErr= _outBuff[QProcess::StandardError].first(_readyBeginPos[QProcess::StandardError]);

        // Remove extracted data from buffers
        _outBuff[QProcess::StandardOutput].remove(0, _readyEndPos[QProcess::StandardOutput]+1);
        _outBuff[QProcess::StandardError].remove( 0, _readyEndPos[QProcess::StandardError]+1);

        // Check if some data remain in buffers (i.e., after '{ready###}') then print warning (should never happen)
        if(_outBuff[QProcess::StandardOutput].size() || _outBuff[QProcess::StandardError].size()) {
            qWarning() << "ZExifToolProcess::onReadyRead: WARNING: Some data exists after {ready###}";
            qWarning() << "    StdOut content:" << _outBuff[QProcess::StandardOutput];
            qWarning() << "    StdErr content:" << _outBuff[QProcess::StandardError];
        }

        // Emit complete message
        qint64 execTime= _execTimer->elapsed();
        emit cmdCompleted(cmdIdStdOut, stdOut, stdErr, execTime);
        //qDebug() << "cmdIDs:" << cmdIdStdOut << "StdOutBuffSize:" << _outBuff[QProcess::StandardOutput].size() << "StdErrBuffSize:" << _outBuff[QProcess::StandardError].size() << "execTime:" << execTime;
    }

    _cmdRunning= false; // No command is running
    execNextCmd();      // Exec next command
}

void ZExifToolProcess::setProcessErrorAndEmit(QProcess::ProcessError error, const QString &description) {
    _processError= error;
    _errorString=  description;
    emit errorOccurred(error);
}



void ZExifToolProcess::onErrorOccurred(QProcess::ProcessError error) {
    setProcessErrorAndEmit(error, _process->errorString());
}

void ZExifToolProcess::onFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    // Reset running variables
    _pendingTerminate= false;
    _cmdRunning=       false;
    _cmdQueue=         QByteArrayList();

    // Free stdOut and stdErr buffers
    _process->readAllStandardOutput();
    _process->readAllStandardError();
    _readyBeginPos[0]= _readyBeginPos[1]= -1;
    _readyEndPos[0]=   _readyEndPos[1]=   -1;
    _outBuff[0]=       _outBuff[1]=    QByteArray();

    emit finished(exitCode, exitStatus);
}
