#ifndef EXAMPLE3_H
#define EXAMPLE3_H

// Qt Core
#include <QMap>

// Qt Widget
#include <QWidget>

// Internal
#include "ZExifTool/ZExifToolProcess.h"

namespace Ui {
   class Example3;
}

class Example3 : public QWidget {
    Q_OBJECT

// ENUMS
public:
    enum Summary {
        SummaryDirectoriesScanned,   // Directories scanned
        SummaryDirectoriesCreated,   // Directories created
        SummaryFilesFailedCondition, // Files failed condition
        SummaryImageFilesCreated,    // Image files created
        SummaryImageFilesUpdated,    // Image files updated
        SummaryImageFilesUnchanged,  // Image files unchanged
        SummaryImageFilesMoved,      // Image files moved
        SummaryImageFilesCopied,     // Image files copied
        SummaryFileUpdateErrors,     // Files weren't updated due to errors
        SummaryFileCreateErrors,     // Files weren't created due to errors
        SummaryImageFilesRead,       // Image files read
        SummaryImageFileErrors,      // Files could not be read
        SummaryOutputFilesCreated,   // Output files created
        SummaryOutputFilesAppended,  // Output files appended
        SummaryHardLinksCreated,     // Hard links created
        SummaryHardLinkErrors,       // Hard links could not be created
        SummarySymbolicLinksCreated, // Symbolic links created
        SummarySymbolicLinkErrors,   // Symbolic links could not be created
    };

private:
    enum CmdType {
        GetImgInfo,
        SetImgInfo,
    };

// METHODS
public:
    explicit Example3(QWidget *parent = nullptr);
    ~Example3();

private slots:
    void on_pushButtonStart_clicked();
    void on_pushButtonStop_clicked();
    void on_pushButtonRead_clicked();
    void on_pushButtonWriteDesc_clicked();

    void onEtProcessStarted();
    void onEtProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onEtProcessStateChanged(QProcess::ProcessState newState);
    void onEtProcessErrorOccured(QProcess::ProcessError error);
    void onEtCmdCompleted(int cmdId, const QByteArray &cmdStdOut, const QByteArray &cmdErrOut, qint64 execTime);

private:
    void etReturn_getImgInfo(const QByteArray &cmdOutput, const QByteArray &cmdError);
    void etReturn_setImgInfo(const QByteArray &cmdOutput, const QByteArray &cmdError);
    int  getSummary(const QByteArray &output, Example3::Summary summary);

// VARIABLES
private:
    Ui::Example3 *ui;
    ZExifToolProcess *etProcess= 0;
    QMap<int, CmdType> _queuedCmdType;
};

#endif // EXAMPLE3_H
