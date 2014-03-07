#-------------------------------------------------
#
# Project created by QtCreator 2014-03-07T10:53:08
#
#-------------------------------------------------
TEMPLATE = lib

CONFIG += dll

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CadastrTools

DLLDESTDIR = $$PWD/bin

RELEASE_WITH_DEBUGINFO {
    QMAKE_CXXFLAGS_RELEASE = $$QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO
    QMAKE_LFLAGS_RELEASE   = $$QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO
}

# Use Precompiled headers (PCH)
CONFIG += precompile_header
PRECOMPILED_HEADER  = stable.h

DEFINES += CADASTRTOOLS_LIBRARY

contains(CONFIG, static) {
    QTPLUGIN += qwindows
    DEFINES  += STATIC_BUILD
}

win32 {
    QMAKE_EXTENSION_SHLIB = arx

    ## Windows common build here

    INCLUDEPATH += $$ARXSDK\inc

    LIBS += -lrxapi

    contains(ARXSDK, .*2014.*) {
        LIBS += -laccore
        LIBS += -lacdb19
        LIBS += -lacge19
        LIBS += -lac1st19

        TARGET = $$join(TARGET,,,"2014")
    } else {
        LIBS += -lacad
        LIBS += -lacdb18
        LIBS += -lacge18

        TARGET = $$join(TARGET,,,"2012")
    }

    DEF_FILE = $$ARXSDK\inc\AcRxDefault.def

    !contains(QMAKE_HOST.arch, x86_64) {

        ## Windows x86 (32bit) specific build here

        INCLUDEPATH += $$ARXSDK\inc-win32
        LIBPATH     += $$ARXSDK\lib-win32
        DEPENDPATH  += $$ARXSDK\lib-win32

        TARGET = $$join(TARGET,,,"x86")

    } else {

        ## Windows x64 (64bit) specific build here

        INCLUDEPATH += $$ARXSDK\inc-x64
        LIBPATH     += $$ARXSDK\lib-x64
        DEPENDPATH  += $$ARXSDK\lib-x64

        TARGET = $$join(TARGET,,,"x64")
    }
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}


SOURCES += cadastrtools.cpp \
    commands.cpp \
    xmlutil.cpp

HEADERS += cadastrtools.h\
    stable.h \
    commands.h \
    xmlutil.h

