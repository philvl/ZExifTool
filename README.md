# ZExifTool - Qt5 and Qt6 interface for exiftool
Qt5 and Qt6 interface for exiftool by Phil Harvey


## Description
ZExifTool provides a simple way to use exiftool by Phil Harvey in your Qt5/Qt6 applications.
Have a look at the code and the examples, it's very easy!

ZExifTool have the same feature as C ++ Interface for ExifTool (by Phil Harvey) such as:
- Startup overhead avoidance by using the exiftool -stay_open feature;
- Accessibility to all features of the exiftool application;
- Queuing operations and processes them sequentially;
- Easy parallelization for performance-demanding applications.


## Qt Compatibility
ZExifTool has been tested on Windows, Linux and MacOs with:
- Qt 5.9.9   / c++11
- Qt 5.12.10 / c++11
- Qt 5.15.2  / c++11, c++14, c++17
- Qt 6.0.1   / c++11, c++14, c++17


## Installation
The easiest way to include ZExifTool in your project is to copy the ZExifTool folder to your project tree and to add the following include() to your Qt project (.pro) file:

    include("ZExifTool/ZExifTool.pri")

Now, you are ready!


## Usage

First of all, you need to download exiftool at https://exiftool.org.
- For Linux and macOSX version I suggest you extract the exiftool directory and move it to the same place of your application executable. Don't forget to give  permission to the executable on exiftool program.
- For Windows version I suggest you extract the exiftool.exe file and move it to the same directory as your application executable.
### Quick start
```c++
#include "ZExifTool/ZExifToolProcess.h"

// Create a ZExifToolProcess instance
etProcess= new ZExifToolProcess(this);
#if defined Q_OS_LINUX || defined Q_OS_MACOS
    etProcess->setProgram(QLatin1String("./exiftool/exiftool"));
#elif defined Q_OS_WINDOWS
    etProcess->setProgram(QLatin1String("./exiftool.exe"));
#endif

// Connect at least cmdCompleted signal to slot
connect(etProcess, &ZExifToolProcess::cmdCompleted, this, &MyClass::onCmdCompleted);

// Start ZExifToolProcess
etProcess->start();
if(!etProcess->waitForStarted(500)) {
    etProcess->kill();
    return;
}

// Build command (get metadata as JSON array)
QByteArrayList cmdArgs;
cmdArgs << "-json";
cmdArgs << "-binary";
cmdArgs << "-G0";
cmdArgs << "myImage.jpg";

// Send command to ZExifToolProcess
int cmdId= etProcess->command(cmdArgs); // See additional notes
```



```c++
// This slot is called on exiftool command completed
//-- cmdId: is the command ID
//-- execTime: command execution time in ms
//-- stdOut: exiftool standard output
//-- stdErr: exiftool error output
void MyClass::onCmdCompleted(int cmdId, int execTime, const QByteArray &stdOut, const QByteArray &stdErr) {
    // Convert JSON array as QVariantMap
    QJsonDocument jsonDoc=     QJsonDocument::fromJson(cmdOutputChannel);
    QJsonArray    jsonArray=   jsonDoc.array();
    QJsonObject   jsonObject=  jsonArray.at(0).toObject();
    QVariantMap   metadataMap= jsonObject.toVariantMap();

    qDebug() << metadataMap.value("EXIF:Make" );
    qDebug() << metadataMap.value("EXIF:Model");
}
```


## Full ZExitToolProcess public interface
```c++
class ZExifToolProcess : public QObject {
public:
    // Builds a ZExifToolProcess object with the given parent.
    ZExifToolProcess(QObject *parent= nullptr);
    
    // Destructs the ZExifToolProcess object, i.e., killing the process.
    ~ZExifToolProcess();
    
    // Attempts to terminate the process.
    void setProgram(const QString &etExePath, const QString &perlExePath= QString());
    
    // Starts exiftool in a new process.
    void start();
    
public slots:
    // Attempts to terminate the process.
    void terminate();
    
    // Kills the current process, causing it to exit immediately.
    void kill();
    
public:
    // Returns true if ZExifToolProcess is running (process state == Running)
    bool isRunning() const;
    
    // Returns true if a command is running
    bool isBusy() const;

    // Returns the native process identifier for the running process, if available. If no process currently running, 0 is returned
    qint64 processId() const;
    
    // Returns the current state of the process.
    QProcess::ProcessState state() const;
    
    // Returns the type of error that occurred last.
    QProcess::ProcessError error() const;
    
    // Returns an error message
    QString errorString() const;
    
    // Returns the exit status of the last process that finished.
    QProcess::ExitStatus exitStatus() const;
    int exitCode() const;
    
    // Blocks until the process has started and the started() signal has been emitted, or until msecs milliseconds have passed.
    bool waitForStarted(int msecs= 30000);
    
    // Blocks until the process has finished and the finished() signal has been emitted, or until msecs milliseconds have passed.
    bool waitForFinished(int msecs= 30000);

    // Sends a command to exiftool process
    int command(const QByteArrayList &args);
    
signals:
    // This signal is emitted by ZExitToolProcess when the process has started, and state() returns Running.
    void started();
    
    // This signal is emitted when the ZExitToolProcess finishes.
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    
    // This signal is emitted whenever the state of ZExitToolProcess changes.
    void stateChanged(QProcess::ProcessState newState);
    
    // This signal is emitted when an error occurs with the process. 
    void errorOccurred(QProcess::ProcessError error);

    // This slot is called on exiftool command completed
    void cmdCompleted(int cmdId, int execTime, const QByteArray &cmdOutputChannel, const QByteArray &cmdErrorChannel);
}
```


## Additional notes
The "etProcess->command(cmdArgs)" function ensures the returned command ID is unique. The following context is supported:
- ZExifToolProcess class is instantiated severial times;
- ZExifToolProcess is used is multithread environment;
- Multiple instance of exiftool program are running at the same time.


## Example
The following repository contains three commented examples based on ZExifToolProcess.
Open ZExifToolExample.pro in QtCreator and run it or read sources for more information.
![Example 1](https://zupimages.net/up/21/07/5eum.png)
![Example 2](https://zupimages.net/up/21/07/tbeo.png)


## Support
I do not provide any specific support, but if you have any question, please feel free to reach me out via email at philvl.dev(at)gmail.com
