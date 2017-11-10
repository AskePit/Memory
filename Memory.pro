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
    utils.cpp \
    highlighters/clike.cpp \
    highlighters/cplusplus.cpp \
    highlighters/js.cpp \
    highlighters/python.cpp \
    highlighters/tab.cpp \
    RunGuard.cpp \
    syntax.cpp

HEADERS  += mainwindow.h \
    dirmodel.h \
    eventfilter.h \
    texteditor.h \
    utils.h \
    highlighters/clike.h \
    highlighters/cplusplus.h \
    highlighters/js.h \
    highlighters/python.h \
    highlighters/tab.h \
    highlighters/highlighters.h \
    RunGuard.h \
    syntax.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources/resources.qrc
