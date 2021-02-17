QT     += core widgets gui
CONFIG += c++17 # c++11, c++14 or c++17

# TARGET = Examples
TEMPLATE = app

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    Examples/main.cpp \
    Examples/MainDialog.cpp \
    Examples/Example1.cpp \
    Examples/Example2.cpp \
    Examples/Example3.cpp

HEADERS += \
    Examples/MainDialog.h \
    Examples/Example1.h \
    Examples/Example2.h \
    Examples/Example3.h

FORMS += \
    Examples/Example1.ui \
    Examples/Example2.ui \
    Examples/Example3.ui

RESOURCES += \
    Examples/Resources.qrc

DISTFILES += \
    CHANGELOG.md \
    LICENSE.md \
    README.md

# Add the following line to your project
include(ZExifTool/ZExifTool.pri)


###
# PVL preferences
###
macx {
    # Do to NOT puts the executable into a bundle
    CONFIG -= app_bundle
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
