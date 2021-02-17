#ifndef EXAMPLE1_H
#define EXAMPLE1_H

// Qt Widget
#include <QWidget>

// ZExifTool
#include "ZExifTool/ZExifToolProcess.h"

namespace Ui {
    class Example1;
}

class Example1 : public QWidget {
    Q_OBJECT

// METHODS
public:
    explicit Example1(QWidget *parent = nullptr);
    ~Example1();

private slots:
    void on_pushButtonRead_clicked();

    // ZExifToolProcess slots
    void onEtProcessStarted();
    void onEtProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onEtProcessStateChanged(QProcess::ProcessState newState);
    void onEtProcessErrorOccured(QProcess::ProcessError error);
    void onEtCmdCompleted(int cmdId, int execTime, const QByteArray &cmdOutputChannel, const QByteArray &cmdErrorChannel);

// VARIABLES
private:
    Ui::Example1 *ui;
    ZExifToolProcess *etProcess= 0;
};

#endif // EXAMPLE1_H
