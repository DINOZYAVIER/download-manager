QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = DownloadManager
TEMPLATE = app

# The following define makes your compiler emit warnings if you use any feature of Qt
# which has been marked as deprecated. Please consult the documentation in order to know how fix it.
DEFINES += QT_DEPRECATED_WARNINGS

# Generate make file only for certain build type
CONFIG -= debug_and_release

# Set app version
REV = $$system(git rev-list --count HEAD)
BUILD_VERSION = 0.1.0.$$REV
VERSION = $$BUILD_VERSION

LIBS += -L/usr/local/opt/openssl@1.1/lib

# Add include path (simplify include in the source files)
PRECOMPILED_HEADER = precompiled.h

INCLUDEPATH += \
    $$PWD/sources/

SOURCES += \
    sources/controller.cpp \
    sources/downloader.cpp \
    sources/downloadtablemodel.cpp \
    sources/main.cpp \
    sources/mainwindow.cpp

HEADERS += \
    sources/controller.h \
    sources/downloader.h \
    sources/downloadtablemodel.h \
    sources/mainwindow.h \
    sources/precompiled.h

FORMS += \
    forms/mainwindow.ui \

win32: PLATFORM = "windows"
linux: PLATFORM = "linux"
macx:  PLATFORM = "mac"

DESTDIR = $$PWD/bin/$${PLATFORM}_$${BUILD_CONFIG}/

# Deployment
win32 {
    DEPLOY_TARGET = $$shell_quote($$DESTDIR/$${TARGET}.exe)
    DEPLOY_COMMAND = windeployqt --no-compiler-runtime --no-opengl-sw
    QMAKE_TARGET_COMPANY = NixSolutions LLC
    QMAKE_TARGET_PRODUCT = Download Manager App
    QMAKE_TARGET_DESCRIPTION = Download Manager App
    QMAKE_TARGET_COPYRIGHT = "NixSolutions LLC \\251 2020"
}

macx {
    DEPLOY_TARGET = $$shell_quote($$DESTDIR/$${TARGET}.app)
    DEPLOY_COMMAND = macdeployqt
    #QMAKE_INFO_PLIST=./resources/hospital.plist
}

win32|macx {
    # Generate release
    CONFIG(release, debug|release): QMAKE_POST_LINK = $${DEPLOY_COMMAND} $${DEPLOY_TARGET}
}


