#include "MainWindow.h"

#include <QDir>
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("ZExifTool Examples");
    QDir::setCurrent(qApp->applicationDirPath());
/*
    // Quick dark theme
    //-- Force the style to be the same on oll OSs
    app.setStyle(QStyleFactory::create("Fusion"));

    //-- Now use a palette to switch to dark colors:
    QPalette palette= qApp->palette();
    //-- Original QT Creator 4.13.3 Design Dark theme (2020-11-13)
    //-- Values extracted from: qt/Tools/QtCreator/share/qtcreator/themes/design.creatortheme
    //-------------- ColorGroup -------- ColorRole ---------------- Color ----------------------
    palette.setColor(                    QPalette::Window,          QColor( 38,  38,  38     ));
    palette.setColor(QPalette::Disabled, QPalette::Window,          QColor( 68,  68,  68     ));
    palette.setColor(                    QPalette::WindowText,      QColor(218, 218, 218     ));
    palette.setColor(QPalette::Disabled, QPalette::WindowText,      QColor(164, 166, 168,  96));
    palette.setColor(                    QPalette::Base,            QColor( 38,  39,  40     ));
    palette.setColor(QPalette::Disabled, QPalette::Base,            QColor( 68,  68,  68     ));
    palette.setColor(                    QPalette::AlternateBase,   QColor( 53,  54,  55     ));
    palette.setColor(QPalette::Disabled, QPalette::AlternateBase,   QColor( 53,  54,  55     ));
    palette.setColor(                    QPalette::ToolTipBase,     QColor( 17,  17,  17     ));
    palette.setColor(QPalette::Disabled, QPalette::ToolTipBase,     QColor( 17,  17,  17     ));
    palette.setColor(                    QPalette::ToolTipText,     QColor(218, 218, 218     ));
    palette.setColor(QPalette::Disabled, QPalette::ToolTipText,     QColor(218, 218, 218     ));
#if QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    palette.setColor(                    QPalette::PlaceholderText, QColor(208, 208, 208, 128));
    palette.setColor(QPalette::Disabled, QPalette::PlaceholderText, QColor(208, 208, 208, 128));
#endif
    palette.setColor(                    QPalette::Text,            QColor(218, 218, 218     ));
    palette.setColor(QPalette::Disabled, QPalette::Text,            QColor(164, 166, 168,  96));
    palette.setColor(                    QPalette::Button,          QColor( 38,  38,  38     ));
    palette.setColor(QPalette::Disabled, QPalette::Button,          QColor( 38,  38,  38     ));
    palette.setColor(                    QPalette::ButtonText,      QColor(218, 218, 218     ));
    palette.setColor(QPalette::Disabled, QPalette::ButtonText,      QColor(164, 166, 168,  96));
    palette.setColor(                    QPalette::BrightText,      QColor(255,  51,  51     ));
    palette.setColor(QPalette::Disabled, QPalette::BrightText,      QColor(255,  51,  51     ));
    palette.setColor(                    QPalette::Highlight,       QColor( 31, 117, 204, 170));
    palette.setColor(QPalette::Disabled, QPalette::Highlight,       QColor( 31, 117, 204, 170));
    palette.setColor(                    QPalette::HighlightedText, QColor(255, 255, 255     ));
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText, QColor(255, 255, 255     ));
    palette.setColor(                    QPalette::Link,            QColor(  0, 122, 244     ));
    palette.setColor(QPalette::Disabled, QPalette::Link,            QColor(  0, 122, 244     ));
    palette.setColor(                    QPalette::LinkVisited,     QColor(165, 122, 255     ));
    palette.setColor(QPalette::Disabled, QPalette::LinkVisited,     QColor(165, 122, 255     ));
    //-- Extra palette
    palette.setColor(                    QPalette::Light,           QColor(255, 255, 255     ));
    palette.setColor(                    QPalette::Midlight,        QColor(202, 202, 202     ));
    palette.setColor(                    QPalette::Dark,            QColor( 19,  19,  19     ));
    palette.setColor(                    QPalette::Mid,             QColor(184, 184, 184     ));
    palette.setColor(                    QPalette::Shadow,          QColor(118, 118, 118     ));
    //-- Apply palette
    app.setPalette(palette);
//*/
    MainWindow mainDialog;
    mainDialog.show();
    return app.exec();
}
