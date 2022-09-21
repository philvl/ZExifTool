INCLUDEPATH += $$PWD

lessThan(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 15) {
   warning(ZCodeEditor require at least Qt 5.15)
}

SOURCES += \
    $$PWD/ZCodeEditor.cpp

HEADERS += \
    $$PWD/ZCodeEditor.h
