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

#ifndef Z_EXIFTOOL_PROCESS_H
#define Z_EXIFTOOL_PROCESS_H

// Qt Core
#include <QObject>
#include <QElapsedTimer>
#include <QProcess>
#include <QMutex>

class ZExifToolProcess : public QObject {
    Q_OBJECT
// STRUCTS
private:
    struct Command {
        int id;
        QByteArray argsStr;
    };

// METHODS
public:
    ZExifToolProcess(QObject *parent= nullptr);
    ~ZExifToolProcess();
    void setProgram(const QString &etExePath, const QString &perlExePath= QString());
    void start();

public slots:
    void terminate();
    void kill();

public:
    bool isRunning() const;
    bool isBusy() const;
    //--
    qint64 processId() const;
    QProcess::ProcessState state() const;
    QProcess::ProcessError error() const;
    QString                errorString() const;
    QProcess::ExitStatus   exitStatus() const;
    int exitCode() const;
    //--
    bool waitForStarted(int msecs= 30000);
    bool waitForFinished(int msecs= 30000);
    //bool waitForCompleted(int msecs= 30000); // TODO: Need to be implemented

    int command(const QByteArrayList &args);
private:
    void execNextCmd();

private slots:
    void onStarted();
    void onFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onStateChanged(QProcess::ProcessState newState);
    void onErrorOccurred(QProcess::ProcessError error);
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
private:
    void readOutput(const QProcess::ProcessChannel channel);
    void setProcessErrorAndEmit(QProcess::ProcessError error, const QString &description);

signals:
    void started();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void stateChanged(QProcess::ProcessState newState);
    void errorOccurred(QProcess::ProcessError error);
    //--
    void cmdCompleted(int cmdId, int execTime, const QByteArray &cmdOutputChannel, const QByteArray &cmdErrorChannel);

// VARIABLES
private:
    static       QMutex _cmdIdMutex;
    static const int    CMD_ID_MIN;
    static const int    CMD_ID_MAX;
    static       int    _nextCmdId; // Unique identifier, even in a multi-instances or multi-thread environment

    QString   _etExePath;
    QString   _perlExePath;
    QProcess *_process;

    QElapsedTimer  _execTimer;
    QList<Command> _cmdQueue;
    int            _cmdRunning;

    int        _outAwait[2]; // [0] StandardOutput | [1] ErrorOutput
    bool       _outReady[2]; // [0] StandardOutput | [1] ErrorOutput
    QByteArray _outBuff[2];  // [0] StandardOutput | [1] ErrorOutput

    bool _writeChannelIsClosed;

    QProcess::ProcessError _processError;
    QString _errorString;
};

#endif // Z_EXIFTOOL_PROCESS_H
