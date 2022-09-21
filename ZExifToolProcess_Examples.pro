QT     += core widgets gui
CONFIG += c++17 # c++11, c++14 or c++17

# TARGET = Examples
TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS


SOURCES += \
    ZExifToolProcess_Examples/main.cpp \
    ZExifToolProcess_Examples/MainWindow.cpp \
    ZExifToolProcess_Examples/Example1.cpp \
    ZExifToolProcess_Examples/Example2.cpp \
    ZExifToolProcess_Examples/Example3.cpp

HEADERS += \
    ZExifToolProcess_Examples/MainWindow.h \
    ZExifToolProcess_Examples/Example1.h \
    ZExifToolProcess_Examples/Example2.h \
    ZExifToolProcess_Examples/Example3.h

FORMS += \
    ZExifToolProcess_Examples/Example1.ui \
    ZExifToolProcess_Examples/Example2.ui \
    ZExifToolProcess_Examples/Example3.ui

RESOURCES += \
    zResources/Resources.qrc

DISTFILES += \
    CHANGELOG.md \
    LICENSE.md \
    README.md


# ZExifToolProcess: Add the following line to your project
include(ZExifTool/Lib_ZExifTool.pri)


###
# PVL preferences
###
macx {
    # Do to NOT puts the executable into a bundle
    CONFIG -= app_bundle

    # Silence untested Qt version with MacOs SDK warning
    CONFIG += sdk_no_version_check
}
linux {
    # For Ubuntu - Prevent from executable recognizes as shared library
    # The behavior is occurring because newer ubuntu distros set GCC default link flag -pie, which marks
    # e_type as ET_DYN on the binary file. Consequently, the Operating System recognizes as Shared Library.
    QMAKE_LFLAGS += -no-pie
}

###
# Default rules for deployment.
###
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
