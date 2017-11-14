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

include( ../askelib/public.pri )

INCLUDEPATH += $${ASKE_INCLUDE_PATH}

LIBS += -L$${ASKE_LIB_PATH} -laskelib_std$${ASKE_LIB_SUFFIX} -laskelib_widgets$${ASKE_LIB_SUFFIX}
