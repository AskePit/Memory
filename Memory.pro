#-------------------------------------------------
#
# Project created by QtCreator 2016-12-20T23:11:57
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Memory
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    memorymodel.cpp \
    listeventfilter.cpp

HEADERS  += mainwindow.h \
    memorymodel.h \
    listeventfilter.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources/resources.qrc
