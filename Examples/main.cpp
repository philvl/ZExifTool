#include "MainDialog.h"

#include <QDir>
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("ZExifTool Examples");
    QDir::setCurrent(qApp->applicationDirPath());

    MainDialog mainDialog;
    mainDialog.show();
    return app.exec();
}
