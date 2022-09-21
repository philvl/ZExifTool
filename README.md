# ZExifTool - Qt5 and Qt6 interface for exiftool
Qt6 interface for exiftool by Phil Harvey


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
- Qt 6.0 / c++11, c++14, c++17
- Qt 6.2 / c++11, c++14, c++17


## Installation
The easiest way to include ZExifTool in your project is to copy the ZExifTool folder to your project tree and to add the following include() to your Qt project (.pro) file:

    include("ZExifTool/Lib_ZExifTool.pri")

Now, you are ready!


## Usage

First of all, you need to download exiftool at https://exiftool.org.
- For Linux and macOSX version I suggest you extract the exiftool directory and move it to the same place of your application executable. Don't forget to give  permission to the executable on exiftool program.
- For Windows version I suggest you extract the exiftool.exe file and move it to the same directory as your application executable.
### Quick start
```c++
#include "ZExifTool/ZExifToolProcess.h"

// Create a ZExifToolProcess instance
#ifdef Q_OS_WINDOWS
  QString etExePath=   QLatin1String("./exiftool.exe");
#elif defined Q_OS_LINUX || defined Q_OS_MACOS
  QString etExePath=   QLatin1String("./exiftool/exiftool");
#endif
etProcess= new ZExifToolProcess(etExePath, QString(), this);

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
//-- cmdId:     is the command ID
//-- cmdStdOut: exiftool standard output
//-- cmdErrOut: exiftool error output
//-- execTime:  command execution time in ms
void MyClass::onCmdCompleted(int cmdId, const QByteArray &cmdStdOut, const QByteArray &cmdErrOut, qint64 execTime) {
    // Convert JSON array as QVariantMap
    QJsonDocument jsonDoc=     QJsonDocument::fromJson(cmdStdOut);
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
    // Constructs a ZExifToolProcess object with exiftool executable path, perl executable path and the given parent.
    // Internally, set the startup arguments of the exiftool process ("-stay_open true -@ -").
    // Each new ZExifToolProcess object maintains an independent exiftool process/application.
    ZExifToolProcess(const QString &etExePath, const QString &perlExePath= QString(), QObject *parent= nullptr);
    
    // Destructs the ZExifToolProcess object, i.e., killing exiftool process immediately.
    // WARNING: This function may corrupt files if exiftool performs write operations.
    ~ZExifToolProcess();
    
public slots:
    // Start the exiftool process
    void start();
    
    // Attempts to terminate the exiftool process gracefully.
    // Flush the command queue, then instructs exiftool to terminate after the current command is completed.
    // After calling this function, sending commands to exiftool becomes impossible.
    // This function returns immediately. If you want to wait for the exiftool process to finish, use waitForFinished() after terminate().
    void terminate();
    
    // Terminate the exiftool process gracefully.
    // Flush the command queue, then blocks until the current command is completed, the exiftool process has finished and the finished() signal has been emitted, or until msecs milliseconds have passed (Default: no timeout).
    // Calling this function from the main (GUI) thread might cause your user interface to freeze.
    void terminateSafely(int msecs= -1) {
    
    // Kills the exiftool process, causing it to exit immediately.
    // WARNING: This function may corrupt files if exiftool performs write operations.
    void kill();
    
public:
    // Returns true if the exiftool process is running
    bool isRunning() const;
    
    // Returns true if the exiftool process is busy, i.e., a command is running
    bool isBusy() const;
    
    // Returns the command line arguments the exiftool process start with.
    QStringList arguments() const;
    
    // Returns the type of error that occurred last (FailedToStart, Crashed, Timedout, ReadError, WriteError or UnknownError).
    QProcess::ProcessError error() const;
    
    // Returns a human-readable description of the last error that occurred.
    QString errorString() const;

    // Returns the exit status of the last exiftool process that finished (NormalExit | CrashExit).
    // On Windows, if the process was terminated with TerminateProcess() from another application, this function will still return NormalExit unless the exit code is less than 0
    QProcess::ExitStatus exitStatus() const;

    // Returns the exit code of the last exiftool process that finished.
    // This value is not valid unless exitStatus() returns NormalExit.
    int exitCode() const;

    // Returns the native process identifier for the running exiftool process, if available. If no process is currently running, 0 is returned.
    qint64 processId() const;
    
    // Returns the current state of exiftool process (NotRunning, Starting or Running).
    QProcess::ProcessState state() const;
    
    // Blocks until exiftool process has started and the started() signal has been emitted, or until msecs milliseconds have passed (Default: 1 sec timeout).
    bool waitForStarted(int msecs= 1000);
    
    // Blocks until exiftool process has finished and the finished() signal has been emitted, or until msecs milliseconds have passed (Default: no timeout).
    bool waitForFinished(int msecs= -1);

    // Sends the command to exiftool and returns a unique command ID.
    // This function is thread-safe, command ID is unique even in threaded environment
    // Return 0 on errors (ExitTool not running, or args is empty)
    quint32 command(const QByteArrayList &args);
    
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
    void cmdCompleted(int cmdId, const QByteArray &cmdStdOut, const QByteArray &cmdErrOut, qint64 execTime);
}
```


## Additional notes
The "etProcess->command(cmdArgs)" function ensures the returned command ID is unique. The following context is supported:
- ZExifToolProcess class is instantiated severial times;
- ZExifToolProcess is used is multithread environment;
- Multiple instance of exiftool program are running at the same time.


## Example
The following repository contains three commented examples based on ZExifToolProcess.
Open ZExifToolProcess_Examples.pro in QtCreator and run it or read sources for more information.
![Example 1](https://zupimages.net/up/21/07/5eum.png)
![Example 2](https://zupimages.net/up/21/07/tbeo.png)


## Support
I do not provide any specific support, but if you have any question, please feel free to reach me out via email at philvl.dev(at)gmail.com
