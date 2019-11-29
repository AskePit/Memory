#-------------------------------------------------
#
# Project created by QtCreator 2016-12-20T23:11:57
#
#-------------------------------------------------

QT += core gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Memory
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    dirmodel.cpp \
    eventfilter.cpp \
    utils.cpp \
    RunGuard.cpp

HEADERS  += mainwindow.h \
    dirmodel.h \
    eventfilter.h \
    utils.h \
    RunGuard.h

FORMS += mainwindow.ui

RESOURCES += \
    resources/resources.qrc

include( ../askelib_qt/public.pri )
include( ../askelib_qt/askelib/public.pri )

INCLUDEPATH += ../
INCLUDEPATH += ../askelib_qt

LIBS += -L$${ASKELIBQT_LIB_PATH} -laskelib_qt_std$${ASKELIBQT_LIB_SUFFIX} -laskelib_qt_widgets$${ASKELIBQT_LIB_SUFFIX}
LIBS += -L$${ASKELIB_LIB_PATH} -laskelib_std$${ASKELIB_LIB_SUFFIX}
