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

#ifndef ZEXIFTOOL_H
#define ZEXIFTOOL_H

#include <QObject>
// Internal
#include "ZExifToolProcess.h"

class ZExifTool : public QObject {
    Q_OBJECT
public:
    explicit ZExifTool(const QString &etExePath, const QString &perlExePath= QString(), QObject *parent= nullptr);
    ~ZExifTool();

    int getImgInfo(const QString &file, const QByteArrayList &extraArgs= QByteArrayList());

private slots:
    void onEt_started();
    void onEt_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void onEt_stateChanged(QProcess::ProcessState newState);
    void onEt_errorOccurred(QProcess::ProcessError error);
    //void onEt_buffNewData(QProcess::ProcessChannel channel, const QByteArray &buffer);
    void onEt_cmdCompleted(int cmdId, const QByteArray &cmdStdOut, const QByteArray &cmdErrOut, qint64 execTime);

signals:
    void cmdCompleted(int cmdId, const QByteArray &cmdStdOut, const QByteArray &cmdErrOut, qint64 execTime);

private:
    ZExifToolProcess *_process;
};

#endif // ZEXIFTOOL_H
