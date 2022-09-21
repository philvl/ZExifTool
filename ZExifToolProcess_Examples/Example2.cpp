#include "Example2.h"
#include "ui_Example2.h"

#include "ZExifTool/ZExifToolProcess.h"

Example2::Example2(QWidget *parent) : QWidget(parent), ui(new Ui::Example2) {
    ui->setupUi(this);
    const QFont fixedFont= QFontDatabase::systemFont(QFontDatabase::FixedFont);
    ui->textEditStdOut->setFont(fixedFont);
    ui->textEditStdErr->setFont(fixedFont);

    // Create ZExifToolProcess
  #ifdef Q_OS_WINDOWS
    QString etExePath=   QLatin1String("./exiftool.exe");
    QString perlExePath= QString();
  #elif defined Q_OS_LINUX || defined Q_OS_MACOS
    QString etExePath=   QLatin1String("./exiftool/exiftool");
    QString perlExePath= QString();
  #endif
    etProcess= new ZExifToolProcess(etExePath, perlExePath, this);
    connect(etProcess, &ZExifToolProcess::started,       this, &Example2::onEtProcessStarted);
    connect(etProcess, &ZExifToolProcess::finished,      this, &Example2::onEtProcessFinished);
    connect(etProcess, &ZExifToolProcess::stateChanged,  this, &Example2::onEtProcessStateChanged);
    connect(etProcess, &ZExifToolProcess::errorOccurred, this, &Example2::onEtProcessErrorOccured);
    connect(etProcess, &ZExifToolProcess::cmdCompleted,  this, &Example2::onEtCmdCompleted);
}

Example2::~Example2() {
    // Terminate exiftool safely, before delete Ui
    etProcess->terminateSafely();
    delete ui;
}

void Example2::on_pushButtonRead_clicked() {
    // Start ZExifToolProcess
    etProcess->start();
    if(!etProcess->waitForStarted(1000)) {
        etProcess->kill();
        return;
    }

    // Build command
    QByteArrayList cmdArgs;
    cmdArgs << "-json";
    if(ui->checkBoxExtractBinary->isChecked())  cmdArgs << "-binary";
    if(ui->checkBoxPrintTagID->isChecked())     cmdArgs << "-hex";
    if(ui->checkBoxPrintDescValue->isChecked()) cmdArgs << "-long";
    cmdArgs << ui->lineEditImgPath->text().toUtf8();

    // Send command to ZExifToolProcess
    etProcess->command(cmdArgs);
}


// This slot is called when ZExifToolProcess process started
void Example2::onEtProcessStarted() {
    ui->pushButtonRead->setEnabled(false);
}

// This slot is called when ZExifToolProcess process finished
void Example2::onEtProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode) Q_UNUSED(exitStatus)
    ui->pushButtonRead->setEnabled(true);
}

// This slot is called when ZExifToolProcess state changed
void Example2::onEtProcessStateChanged(QProcess::ProcessState newState) {
    switch(newState) {
    case QProcess::NotRunning: ui->labelProcessStatus->setText("Not running");       break;
    case QProcess::Starting:   ui->labelProcessStatus->setText("Starting");          break;
    case QProcess::Running:    ui->labelProcessStatus->setText("<b>Running...</b>"); break;
    }
}

// This slot is called on ZExifToolProcess process error, not when exiftool return error
//   Process error: FailedToStart, Crashed, Timedout, ReadError, WriteError and Unknown
void Example2::onEtProcessErrorOccured(QProcess::ProcessError error) {
    ui->labelInfo->setText("<span style='color:#C00000;'>" + etProcess->errorString() + " (errno: " + QString::number(error) + ")</span>");
    etProcess->terminate();
}


// This slot is called on exiftool command completed
//   cmdOutput channel contains exiftool stdout
//   cmdSrror  channel contains exiftool stderr
void Example2::onEtCmdCompleted(int cmdId, const QByteArray &cmdStdOut, const QByteArray &cmdErrOut, qint64 execTime) {
    ui->textEditStdOut->setPlainText(cmdStdOut); // Show result as JSON (stdOut)
    ui->textEditStdErr->setPlainText(cmdErrOut);  // Show error (stdErr)

    // Show info
    ui->labelInfo->setText("Cmd ID: " + QString::number(cmdId) + " - Exec time: " + QString::number(execTime) + "ms");

    // Stop ZExifToolProcess
    etProcess->terminate();
}
