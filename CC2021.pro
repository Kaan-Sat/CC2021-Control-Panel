#-------------------------------------------------------------------------------
# Make options
#-------------------------------------------------------------------------------

UI_DIR = uic
MOC_DIR = moc
RCC_DIR = qrc
OBJECTS_DIR = obj

CONFIG += c++11

#-------------------------------------------------------------------------------
# Qt configuration
#-------------------------------------------------------------------------------

TEMPLATE = app
TARGET = cc2021

CONFIG += qtc_runnable
CONFIG += resources_big
CONFIG += qtquickcompiler

QT += xml
QT += sql
QT += svg
QT += core
QT += quick
QT += widgets
QT += quickcontrols2

QTPLUGIN += qsvg

#-------------------------------------------------------------------------------
# Compiler options
#-------------------------------------------------------------------------------

*g++*: {
    QMAKE_CXXFLAGS_RELEASE -= -O
    QMAKE_CXXFLAGS_RELEASE *= -O3
}

*msvc*: {
    QMAKE_CXXFLAGS_RELEASE -= /O
    QMAKE_CXXFLAGS_RELEASE *= /O2
}
    
#-------------------------------------------------------------------------------
# Libraries
#-------------------------------------------------------------------------------

include(libs/Libraries.pri)

#-------------------------------------------------------------------------------
# Deploy options
#-------------------------------------------------------------------------------

win32* {
    RC_FILE = deploy/windows/resources/info.rc
    OTHER_FILES += deploy/windows/nsis/setup.nsi
}

macx* {
    ICON = deploy/macOS/icon.icns
    RC_FILE = deploy/macOS/icon.icns
    QMAKE_INFO_PLIST = deploy/macOS/info.plist
    CONFIG += sdk_no_version_check # To avoid warnings with Big Sur
}

linux:!android {
    target.path = /usr/bin
    icon.path = /usr/share/pixmaps
    desktop.path = /usr/share/applications
    icon.files += deploy/linux/*.svg
    desktop.files += deploy/linux/*.desktop

    INSTALLS += target desktop icon
}

#-------------------------------------------------------------------------------
# Import source code
#-------------------------------------------------------------------------------

INCLUDEPATH += $$PWD/src

RESOURCES += \
    assets/assets.qrc

DISTFILES += \
    assets/qml/*.qml

HEADERS += \
    src/AppInfo.h \
    src/Misc/Utilities.h \
    src/Misc/TimerEvents.h \
    src/SerialStudio/Communicator.h

SOURCES += \
    src/main.cpp \
    src/Misc/Utilities.cpp \
    src/Misc/TimerEvents.cpp \
    src/SerialStudio/Communicator.cpp
