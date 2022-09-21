#ifndef EXAMPLE2_H
#define EXAMPLE2_H

// Qt Widget
#include <QWidget>

// ZExifTool
#include "ZExifTool/ZExifToolProcess.h"

namespace Ui {
    class Example2;
}

class Example2 : public QWidget {
    Q_OBJECT

// METHODS
public:
    explicit Example2(QWidget *parent = nullptr);
    ~Example2();

private slots:
    void on_pushButtonRead_clicked();

    // ZExifToolProcess slots
    void onEtProcessStarted();
    void onEtProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onEtProcessStateChanged(QProcess::ProcessState newState);
    void onEtProcessErrorOccured(QProcess::ProcessError error);
    void onEtCmdCompleted(int cmdId, const QByteArray &cmdStdOut, const QByteArray &cmdErrOut, qint64 execTime);

// VARIABLES
private:
    Ui::Example2 *ui;
    ZExifToolProcess *etProcess= 0;
};

#endif // EXAMPLE2_H
