#include "MainWindow.h"

#include <QLabel>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QFile>

// Internals
#include "Example1.h"
#include "Example2.h"
#include "Example3.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    resize(760, 520);

    // Create MainWindow central widget
    QWidget *centralWidget= new QWidget;
    setCentralWidget(centralWidget);

    // Perform exiftool executable check and show note is needed
    QString exiftoolExe= "exiftool/exiftool";
  #if defined(Q_OS_WIN)
    exiftoolExe= "exiftool.exe";
  #endif
    QLabel *label= new QLabel(centralWidget);
    label->setOpenExternalLinks(true);
    if(!QFile::exists(exiftoolExe)) {
        label->setText("<b>IMPORTANT NOTE:</b><br/>"
                       "ZExifTool wrapper does not include the exiftool program from Phil Harvey: <a href='https://exiftool.org'>exiftool.org</a>.<br/>"
                       "- On Linux and MacOS: You need to copy the exiftool directory to the same place of this executable.<br/>"
                       "- On Windows: You need to copy exiftool.exe inside the same directory as this executable.");
    }
  #if defined(Q_OS_MACOS) || defined(Q_OS_LINUX)
    else {
        // Set exec permission on exiftool exec file
        QFile::Permissions perms= QFile::permissions(exiftoolExe);
        QFile::setPermissions(exiftoolExe, perms | QFileDevice::ExeOwner);
    }
  #endif

    // Setup Mainwindow Layout
    QTabWidget *tabWidget= new QTabWidget(centralWidget);
    Example1 *example1= new Example1(tabWidget);
    Example2 *example2= new Example2(tabWidget);
    Example3 *example3= new Example3(tabWidget);
    tabWidget->addTab(example1, QString("Example 1"));
    tabWidget->addTab(example2, QString("Example 2"));
    tabWidget->addTab(example3, QString("Example 3"));

    QVBoxLayout *vBoxLayout= new QVBoxLayout(centralWidget);
    vBoxLayout->setContentsMargins(6, 6, 6, 6);
    if(!label->text().isEmpty())
        vBoxLayout->addWidget(label);
    vBoxLayout->addWidget(tabWidget);

    // Extract embedded image into executable dir
    QStringList imgList= QStringList() << "test.jpg" << "test.heic";
    for(const QString &img : qAsConst(imgList)) {
        QFile::remove("./" + img);
        QFile::copy(":/img/" + img, "./" + img);
        QFile::setPermissions("./" + img, QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ReadOther);
    }
}
