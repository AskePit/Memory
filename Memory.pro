#-------------------------------------------------
#
# Project created by QtCreator 2016-12-20T23:11:57
#
#-------------------------------------------------

QT       += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Memory
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dirmodel.cpp \
    eventfilter.cpp \
    texteditor.cpp \
    highlighters/cplusplus.cpp \
    utils.cpp \
    highlighters/clike.cpp \
    highlighters/js.cpp \
    highlighters/tab.cpp \
    RunGuard.cpp

HEADERS  += mainwindow.h \
    dirmodel.h \
    eventfilter.h \
    texteditor.h \
    highlighters/cplusplus.h \
    utils.h \
    highlighters/clike.h \
    highlighters/js.h \
    highlighters/highlighters.h \
    highlighters/tab.h \
    RunGuard.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources/resources.qrc
