/*//
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

#ifndef Z_EXIFTOOL_PROCESS_H
#define Z_EXIFTOOL_PROCESS_H

#include <QObject>
#include <QProcess>
class QElapsedTimer;

class ZExifToolProcess : public QObject {
    Q_OBJECT
public:
    explicit ZExifToolProcess(const QString &etExePath, const QString &perlExePath= QString(), QObject *parent= nullptr);
    ~ZExifToolProcess();

public slots:
    void start();                        // Override QProcess fct
    void terminate();                    // Override QProcess fct
    void terminateSafely(int msecs= -1); // Added funct
    void kill();                         // Call QProcess fct

public:
    bool isRunning() const; // Added funct
    bool isBusy()    const; // Added funct
    //--
    QStringList            arguments()   const; // Call QProcess fct
    QProcess::ProcessError error()       const; // Override QProcess fct
    QString                errorString() const; // Override QProcess fct
    QProcess::ExitStatus   exitStatus()  const; // Call QProcess fct
    int                    exitCode()    const; // Call QProcess fct
    qint64                 processId()   const; // Call QProcess fct
    QProcess::ProcessState state()       const; // Call QProcess fct
    //--
    bool waitForStarted( int msecs= 1000);      // Call QProcess fct
    bool waitForFinished(int msecs=   -1);      // Call QProcess fct

public slots:
    quint32 command(const QByteArrayList &args); // Added funct
private:
    void execNextCmd();

private:
    void readOutput(const QProcess::ProcessChannel channel);
    void setProcessErrorAndEmit(QProcess::ProcessError error, const QString &description);

private slots:
    void onErrorOccurred(QProcess::ProcessError error);
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);

signals:
    void started();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void stateChanged(QProcess::ProcessState newState);
    void errorOccurred(QProcess::ProcessError error);
    //--
    void newBuffData(QProcess::ProcessChannel channel, const QByteArray &rawData); // For debug only
    void cmdCompleted(int cmdId, const QByteArray &cmdStdOut, const QByteArray &cmdErrOut, qint64 execTime);


// VARIABLES
private:
    static const quint32   CMD_ID_MIN;
    static const quint32   CMD_ID_MAX;
    static const qsizetype CMD_ID_LEN;
    static       quint32   _nextCmdId; // Unique identifier, even in a multi-instances or multi-thread environment

    QElapsedTimer *_execTimer;
    QProcess      *_process;

    bool           _pendingTerminate;
    bool           _cmdRunning;
    QByteArrayList _cmdQueue;

    qsizetype  _readyBeginPos[2];  // [0] StandardOutput | [1] ErrorOutput
    qsizetype  _readyEndPos[2];    // [0] StandardOutput | [1] ErrorOutput
    QByteArray _outBuff[2];        // [0] StandardOutput | [1] ErrorOutput

    QProcess::ProcessError _processError;
    QString                _errorString;
};

#endif // Z_EXIFTOOL_PROCESS_H
