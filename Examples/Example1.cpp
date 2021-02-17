#include "Example1.h"
#include "ui_Example1.h"

Example1::Example1(QWidget *parent) : QWidget(parent), ui(new Ui::Example1) {
    ui->setupUi(this);
    const QFont fixedFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    ui->textEditStdOut->setFont(fixedFont);

    // Populate combobox
    //------------------------Description ------------------------------------ Command --
    ui->comboBoxCmd->addItem("Print all valid tag names (-list)",             "-list"  );
    ui->comboBoxCmd->addItem("Print all writable tag names (-listW)",         "-listW" );
    ui->comboBoxCmd->addItem("Print all deletable tag groups (-listD)",       "-listD" );
    ui->comboBoxCmd->addItem("Print all tag in family 0 (-listG0)",           "-listG0");
    ui->comboBoxCmd->addItem("Print all tag in family 1 (-listG1)",           "-listG1");
    ui->comboBoxCmd->addItem("Print all tag in family 2 (-listG2)",           "-listG2");
    ui->comboBoxCmd->addItem("Print all tag in family 3 (-listG3)",           "-listG3");
    ui->comboBoxCmd->addItem("Print all tag in family 4 (-listG4)",           "-listG4");
    ui->comboBoxCmd->addItem("Print all tag in family 5 (-listG5)",           "-listG5");
    ui->comboBoxCmd->addItem("Print all tag in family 6 (-listG6)",           "-listG6");
    ui->comboBoxCmd->addItem("Print all tag in family 7 (-listG7)",           "-listG7");
    ui->comboBoxCmd->addItem("Print all recognized file extensions (-listR)", "-listR" );
    ui->comboBoxCmd->addItem("Print all supported file extensions (-listF)",  "-listF" );
    ui->comboBoxCmd->addItem("Print all writable file extensions (-listWF)",  "-listWF");
    ui->comboBoxCmd->addItem("Print an XML database of tag details (-listX)", "-listX" );
    ui->comboBoxCmd->addItem("Print exiftool version number (-ver)",          "-ver"   );

    // Create ZExifToolProcess
    etProcess= new ZExifToolProcess(this);
  #if defined Q_OS_LINUX || defined Q_OS_MACOS
    etProcess->setProgram(QLatin1String("./exiftool/exiftool"));
  #elif defined Q_OS_WINDOWS
    etProcess->setProgram(QLatin1String("./exiftool.exe"));
  #endif
    connect(etProcess, &ZExifToolProcess::started,       this, &Example1::onEtProcessStarted);
    connect(etProcess, &ZExifToolProcess::finished,      this, &Example1::onEtProcessFinished);
    connect(etProcess, &ZExifToolProcess::stateChanged,  this, &Example1::onEtProcessStateChanged);
    connect(etProcess, &ZExifToolProcess::errorOccurred, this, &Example1::onEtProcessErrorOccured);
    connect(etProcess, &ZExifToolProcess::cmdCompleted,  this, &Example1::onEtCmdCompleted);
}

Example1::~Example1() {
    delete ui;
}

void Example1::on_pushButtonRead_clicked() {
    // Start ZExifToolProcess
    etProcess->start();
    if(!etProcess->waitForStarted(500)) {
        etProcess->kill();
        return;
    }

    // Build command
    QByteArrayList cmdArgs;
    cmdArgs << ui->comboBoxCmd->currentData().toByteArray();

    // Send command to ZExifToolProcess
    etProcess->command(cmdArgs);
}


// This slot is called when ZExifToolProcess process started
void Example1::onEtProcessStarted() {
    ui->comboBoxCmd->setEnabled(false);
    ui->pushButtonRead->setEnabled(false);
}

// This slot is called when ZExifToolProcess process finished
void Example1::onEtProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode) Q_UNUSED(exitStatus)
    ui->comboBoxCmd->setEnabled(true);
    ui->pushButtonRead->setEnabled(true);
}

// This slot is called when ZExifToolProcess state changed
void Example1::onEtProcessStateChanged(QProcess::ProcessState newState) {
    switch(newState) {
    case QProcess::NotRunning: ui->labelProcessStatus->setText("Not running"); break;
    case QProcess::Starting:   ui->labelProcessStatus->setText("Starting");    break;
    case QProcess::Running:    ui->labelProcessStatus->setText("<b>Running...</b>");     break;
    }
}

// This slot is called on ZExifToolProcess process error, not when exiftool return error
//   Process error: FailedToStart, Crashed, Timedout, ReadError, WriteError and Unknown
void Example1::onEtProcessErrorOccured(QProcess::ProcessError error) {
    ui->labelInfo->setText("<span style='color:#C00000;'>" + etProcess->errorString() + " (errno: " + QString::number(error) + ")</span>");
    etProcess->terminate();
}


// This slot is called on exiftool command completed
//   cmdOutput channel contains exiftool stdout
//   cmdSrror  channel contains exiftool stderr
void Example1::onEtCmdCompleted(int cmdId, int execTime, const QByteArray &cmdOutputChannel, const QByteArray &cmdErrorChannel) {
    Q_UNUSED(cmdErrorChannel)

    // Show result as JSON (stdOut)
    ui->textEditStdOut->setPlainText(cmdOutputChannel);

    // Show info
    ui->labelInfo->setText("Cmd ID: " + QString::number(cmdId) + " - Exec time: " + QString::number(execTime) + "ms");

    // Stop ZExifToolProcess
    etProcess->terminate();
}
