#-------------------------------------------------
#
# Project created by QtCreator 2019-01-17T09:54:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OptiPlanner
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    ccenter.cpp \
    ctextelement.cpp \
        main.cpp \
        mainwindow.cpp \
    cadvgraphicsview.cpp \
    cpipe.cpp \
    cgrid.cpp \
    celement.cpp \
    cshaft.cpp \
    cpathelement.cpp \
    cobjectcollection.cpp \
    globals.cpp \
    cinfraprop.cpp \
    cadvscene.cpp \
    routefinderdialog.cpp \
    undocommands.cpp \
    caxises.cpp

HEADERS += \
    ccenter.h \
    ctextelement.h \
        mainwindow.h \
    cadvgraphicsview.h \
    cpipe.h \
    cgrid.h \
    celement.h \
    cshaft.h \
    cpathelement.h \
    cobjectcollection.h \
    globals.h \
    cinfraprop.h \
    cadvscene.h \
    routefinderdialog.h \
    undocommands.h \
    caxises.h

FORMS += \
        mainwindow.ui \
    cinfraprop.ui \
    routefinderdialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

DISTFILES +=
