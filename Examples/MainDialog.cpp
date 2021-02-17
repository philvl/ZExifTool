#include "MainDialog.h"

#include <QLabel>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QFile>

// Internals
#include "Example1.h"
#include "Example2.h"
#include "Example3.h"

MainDialog::MainDialog(QWidget *parent) : QDialog(parent) {
    resize(760, 520);

    QVBoxLayout *vBoxLayout= new QVBoxLayout(this);
    vBoxLayout->setContentsMargins(6, 6, 6, 6);

    QLabel *label= new QLabel(this);
    label->setOpenExternalLinks(true);
    label->setText("<b>IMPORTANT NOTE:</b><br/>"
                   "ZExifTool wrapper does not include the exiftool program from Phil Harvey: <a href='https://exiftool.org'>exiftool.org</a>.<br/>"
                   "On Linux and MacOS: You need to copy the exiftool directory to the same place of this executable.<br/>"
                   "On Windows: You need to copy exiftool.exe inside the same directory as this executable.");

    QTabWidget *tabWidget= new QTabWidget(this);
    Example1 *example1= new Example1(this);
    Example2 *example2= new Example2(this);
    Example3 *example3= new Example3(this);
    tabWidget->addTab(example1, QString("Example 1"));
    tabWidget->addTab(example2, QString("Example 2"));
    tabWidget->addTab(example3, QString("Example 3"));

    vBoxLayout->addWidget(label);
    vBoxLayout->addWidget(tabWidget);

    // Extract embedded image into executable dir
    QFile::remove("./test1.jpg");
    QFile::copy(":/img/test1.jpg", "./test1.jpg");
    QFile::setPermissions("./test1.jpg", QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ReadOther);
}
