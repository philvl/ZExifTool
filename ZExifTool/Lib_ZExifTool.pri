INCLUDEPATH += $$PWD

lessThan(QT_MAJOR_VERSION, 6):lessThan(QT_MINOR_VERSION, 0) {
   warning(ZExifTool require at least Qt 6.0)
}

SOURCES += \
    $$PWD/ZExifToolProcess.cpp

HEADERS += \
    $$PWD/ZExifToolProcess.h
