INCLUDEPATH += $$PWD

equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 9) {
   warning(ZExifTool has not been tested below Qt 5.9, use it as your own risk.)
}

SOURCES += \
    $$PWD/ZExifToolProcess.cpp

HEADERS += \
    $$PWD/ZExifToolProcess.h
