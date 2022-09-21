#include "Example3.h"
#include "ui_Example3.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>

Example3::Example3(QWidget *parent) : QWidget(parent), ui(new Ui::Example3) {
    ui->setupUi(this);
    const QFont fixedFont= QFontDatabase::systemFont(QFontDatabase::FixedFont);
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
    connect(etProcess, &ZExifToolProcess::started,       this, &Example3::onEtProcessStarted);
    connect(etProcess, &ZExifToolProcess::finished,      this, &Example3::onEtProcessFinished);
    connect(etProcess, &ZExifToolProcess::stateChanged,  this, &Example3::onEtProcessStateChanged);
    connect(etProcess, &ZExifToolProcess::errorOccurred, this, &Example3::onEtProcessErrorOccured);
    connect(etProcess, &ZExifToolProcess::cmdCompleted,  this, &Example3::onEtCmdCompleted);
}

Example3::~Example3() {
    // Terminate exiftool safely, before delete Ui
    etProcess->terminateSafely();
    delete ui;
}

void Example3::on_pushButtonStart_clicked() {
    etProcess->start();
}

void Example3::on_pushButtonStop_clicked() {
    etProcess->terminate();
}

void Example3::on_pushButtonRead_clicked() {
    // Build command
    QByteArrayList cmdArgs;
    cmdArgs << "-json";
    cmdArgs << "-binary";
    cmdArgs << "-G0";
    cmdArgs << ui->lineEditImgPath->text().toUtf8();

    // Send command to ZExifToolProcess
    int cmdId= etProcess->command(cmdArgs);
    _queuedCmdType.insert(cmdId, GetImgInfo);
}

void Example3::on_pushButtonWriteDesc_clicked() {
    // Build command
    QByteArrayList cmdArgs;
    cmdArgs << "-json";
    cmdArgs << "-overwrite_original";
    cmdArgs << "-EXIF:ImageDescription=" + ui->lineEditImgDesc->text().toUtf8();
    cmdArgs << ui->lineEditImgPath->text().toUtf8();

    // Send command to ZExifToolProcess
    int cmdId= etProcess->command(cmdArgs);
    _queuedCmdType.insert(cmdId, SetImgInfo);
}


// This slot is called when ZExifToolProcess process started
void Example3::onEtProcessStarted() {
    ui->pushButtonStart->setEnabled(false);
    ui->pushButtonStop->setEnabled(true);
    ui->widget->setEnabled(true);
}

// This slot is called when ZExifToolProcess process finished
void Example3::onEtProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    Q_UNUSED(exitCode) Q_UNUSED(exitStatus)
    ui->pushButtonStart->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);
    ui->widget->setEnabled(false);
}

// This slot is called when ZExifToolProcess state changed
void Example3::onEtProcessStateChanged(QProcess::ProcessState newState) {
    switch(newState) {
    case QProcess::NotRunning: ui->labelProcessStatus->setText("Not running");       break;
    case QProcess::Starting:   ui->labelProcessStatus->setText("Starting");          break;
    case QProcess::Running:    ui->labelProcessStatus->setText("<b>Running...</b>"); break;
    }
}

// This slot is called on ZExifToolProcess process error, not when exiftool return error
//   Process error: FailedToStart, Crashed, Timedout, ReadError, WriteError and Unknown
void Example3::onEtProcessErrorOccured(QProcess::ProcessError error) {
    ui->labelInfo->setText("<span style='color:#C00000;'>" + etProcess->errorString() + " (errno: " + QString::number(error) + ")</span>");
    etProcess->terminate();
}


// This slot is called on exiftool command completed
//   cmdOutput channel contains exiftool stdout
//   cmdError  channel contains exiftool stderr
void Example3::onEtCmdCompleted(int cmdId, const QByteArray &cmdStdOut, const QByteArray &cmdErrOut, qint64 execTime) {
    CmdType cmdType= _queuedCmdType.take(cmdId);
    switch(cmdType) {
    case GetImgInfo:
        etReturn_getImgInfo(cmdStdOut, cmdErrOut);
        break;
    case SetImgInfo:
        etReturn_setImgInfo(cmdStdOut, cmdErrOut);
        break;
    }

    // Show error (stdErr)
    ui->textEditStdErr->setPlainText(cmdErrOut);

    // Show info
    ui->labelInfo->setText("Cmd ID: " + QString::number(cmdId) + " - Exec time: " + QString::number(execTime) + "ms");
}

void Example3::etReturn_getImgInfo(const QByteArray &cmdOutput, const QByteArray &cmdError) {
    Q_UNUSED(cmdError); // In this example, error aren't managed

    QJsonDocument jsonDoc=     QJsonDocument::fromJson(cmdOutput);
    QJsonArray    jsonArray=   jsonDoc.array();
    QJsonObject   jsonObject=  jsonArray.at(0).toObject();
    QVariantMap   metadataMap= jsonObject.toVariantMap();

    ui->labelMake->setText(    metadataMap.value("EXIF:Make",             "N/A").toString());
    ui->labelModel->setText(   metadataMap.value("EXIF:Model",            "N/A").toString());
    ui->labelExposure->setText(metadataMap.value("EXIF:ExposureTime",     "N/A").toString());
    ui->labelFNumber->setText( metadataMap.value("EXIF:FNumber",          "N/A").toString());
    ui->labelIso->setText(     metadataMap.value("EXIF:ISO",              "N/A").toString());
    ui->labelFocal->setText(   metadataMap.value("EXIF:FocalLength",      "N/A").toString());
    ui->labelDesc->setText(    metadataMap.value("EXIF:ImageDescription", "N/A").toString());

    ui->labelThumb->setText("N/A");
    if(metadataMap.value("EXIF:ThumbnailImage").isValid()) {
        QByteArray thumbData= metadataMap.value("EXIF:ThumbnailImage").toByteArray();
        thumbData.remove(0, 7); // Remove "base64:" prefix
        thumbData= QByteArray::fromBase64(thumbData);

        QPixmap pixmap;
        pixmap.loadFromData(thumbData);
        ui->labelThumb->setPixmap(pixmap);
    }
}

void Example3::etReturn_setImgInfo(const QByteArray &cmdOutput, const QByteArray &cmdError) {
    Q_UNUSED(cmdOutput);
    int updatedCount=  getSummary(cmdError, SummaryImageFilesUpdated);
    int noChangeCount= getSummary(cmdError, SummaryImageFilesUnchanged);
    int errorCount=    getSummary(cmdError, SummaryFileUpdateErrors);

    // Checking either the number of updated images or the number of update
    // errors should be sufficient since we are only updating one file,
    // but check both for completeness
    if((updatedCount == 1 || noChangeCount == 1) && errorCount <= 0)
        ui->labelWriteDescStatus->setText("Success");
    else
        ui->labelWriteDescStatus->setText("<span style='color:#C00000;'><b>Error</b></span>");
}

int Example3::getSummary(const QByteArray &output, Example3::Summary summary) {
    QString msg;
    switch(summary) {
    case SummaryDirectoriesScanned:   msg= QLatin1String("directories scanned");                 break;
    case SummaryDirectoriesCreated:   msg= QLatin1String("directories created");                 break;
    case SummaryFilesFailedCondition: msg= QLatin1String("files failed condition");              break;
    case SummaryImageFilesCreated:    msg= QLatin1String("image files created");                 break;
    case SummaryImageFilesUpdated:    msg= QLatin1String("image files updated");                 break;
    case SummaryImageFilesUnchanged:  msg= QLatin1String("image files unchanged");               break;
    case SummaryImageFilesMoved:      msg= QLatin1String("image files moved");                   break;
    case SummaryImageFilesCopied:     msg= QLatin1String("image files copied");                  break;
    case SummaryFileUpdateErrors:     msg= QLatin1String("files weren't updated due to errors"); break;
    case SummaryFileCreateErrors:     msg= QLatin1String("files weren't created due to errors"); break;
    case SummaryImageFilesRead:       msg= QLatin1String("image files read");                    break;
    case SummaryImageFileErrors:      msg= QLatin1String("files could not be read");             break;
    case SummaryOutputFilesCreated:   msg= QLatin1String("output files created");                break;
    case SummaryOutputFilesAppended:  msg= QLatin1String("output files appended");               break;
    case SummaryHardLinksCreated:     msg= QLatin1String("hard links created");                  break;
    case SummaryHardLinkErrors:       msg= QLatin1String("hard links could not be created");     break;
    case SummarySymbolicLinksCreated: msg= QLatin1String("symbolic links created");              break;
    case SummarySymbolicLinkErrors:   msg= QLatin1String("symbolic links could not be created"); break;
    }

    QRegularExpression regExp("([0-9]+) " + msg, QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match= regExp.match(output);
    if(match.hasMatch())
        return match.captured(1).toInt();
    return -1;
}
