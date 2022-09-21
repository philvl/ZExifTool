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

#include "ZExifTool.h"
#include <QFile>
#include <QDebug>


#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonObject>

ZExifTool::ZExifTool(const QString &etExePath, const QString &perlExePath, QObject *parent) : QObject{parent} {
  #if defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
    if(QFile::exists(etExePath)) {
        // Set exec permission on exiftool exec file
        QFile::Permissions perms= QFile::permissions(exiftoolExe);
        QFile::setPermissions(exiftoolExe, perms | QFileDevice::ExeOwner);
    }
  #endif

    _process= new ZExifToolProcess(etExePath, perlExePath, this);
    connect(_process, &ZExifToolProcess::started,       this, &ZExifTool::onEt_started);
    connect(_process, &ZExifToolProcess::finished,      this, &ZExifTool::onEt_finished);
    connect(_process, &ZExifToolProcess::stateChanged,  this, &ZExifTool::onEt_stateChanged);
    connect(_process, &ZExifToolProcess::errorOccurred, this, &ZExifTool::onEt_errorOccurred);
    //connect(_process, &ZExifToolProcess::newBuffData,   this, &ZExifTool::onEt_buffNewData);
    connect(_process, &ZExifToolProcess::cmdCompleted,  this, &ZExifTool::onEt_cmdCompleted);

    _process->start();
}

ZExifTool::~ZExifTool() {
    _process->terminateSafely(); // Terminate exiftool process safetly
}


int ZExifTool::getImgInfo(const QString &file, const QByteArrayList &extraArgs) {
    if(!QFile::exists(file))
        return -1;
    // TODO: check if file has read permission

    // Prepare command to extract the image information
    QByteArrayList args;
    //-- Processing control
    //args << "-fast2";      // Increase speed when extracting metadata | fast1: skip metadata located at the end of file, fast2: skip EXIF MakerNotes too, fast3: keep only file info (get fileType by reading file header), fast4: keep only file info (get fileType based on the file extension)
    //args << "--printConv"; // Disable print conversion for all tags. By default, extracted values are converted to a more human-readable format
    //args << "--composite"; // (-e) Do not generate composite tags (Note: ThumbnailImage and ThumbnailTIFF are composite Tag)
    //-- Input/output text formatting
    args << "-json";         // Export tags in JSON format
    //args << "-long";       // When XML, JSON or PHP format is used, print a description [desc], converted value [val] and unconverted value [num] if it is different from the converted value
    //args << "-binary";       // Output metadata in binary format
    args << "-G0";    // Print group name for each tag | 0: general location, 1: specific location, 2: category, 3: document number), 4: instance number, 5: metadata path, 6: EXIF/TIFF format, 7: tag ID
    //-- Add extraArgs
    if(!extraArgs.isEmpty())
        args << extraArgs;
    //-- File
    args << file.toLatin1();

    int cmdId= _process->command(args);
    //_queuedCmdType.insert(cmdId, GetImgInfo);

    return cmdId;
}



void ZExifTool::onEt_started() {
    qDebug() << "ZExifTool::onStarted()";
}

void ZExifTool::onEt_finished(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug() << "ZExifTool::onFinished" << exitCode << exitStatus;
}

void ZExifTool::onEt_stateChanged(QProcess::ProcessState newState) {
    qDebug() << "ZExifTool::onStateChanged" << newState;
}

void ZExifTool::onEt_errorOccurred(QProcess::ProcessError error) {
    qDebug() << "ZExifTool::onEt_errorOccurred" << error;
}

/*
void ZExifTool::onEt_buffNewData(QProcess::ProcessChannel channel, const QByteArray &buffer) {
}
*/

void ZExifTool::onEt_cmdCompleted(int cmdId, const QByteArray &cmdStdOut, const QByteArray &cmdErrOut, qint64 execTime) {

    QVariantMap   imgData;
    QJsonDocument jsonDoc=    QJsonDocument::fromJson(cmdStdOut);
    QJsonArray    jsonArray=  jsonDoc.array();
    QJsonObject   jsonObject= jsonArray.at(0).toObject();
    qDebug() << jsonDoc;

    for(QJsonObject::const_iterator iterator= jsonObject.constBegin(); iterator != jsonObject.constEnd(); iterator++) {
        QString    objectKey=    iterator.key();
        QJsonValue jsonValue=    iterator.value();
        QVariant   variantValue= jsonValue.toVariant();

        if(jsonValue.isNull())
            variantValue= QLatin1String("<NULL VALUE>");
        if(jsonValue.isString()) {
            QString string= jsonValue.toString();
            if(string.startsWith(QLatin1String("base64:"))) {
                QByteArray base64Str= string.toLatin1();
                base64Str= base64Str.remove(0, 7);
                variantValue= QByteArray::fromBase64(base64Str); // TODO : SEE fromBase64Encoding (since 5.15)
            }
        }
        qDebug() << objectKey << variantValue;
        //imgData.insert(objectKey, variantValue);
    }

    emit cmdCompleted(cmdId, cmdStdOut, cmdErrOut, execTime);
    qDebug() << "ZExifTool::onEt_cmdCompleted" << cmdId << execTime;
}
