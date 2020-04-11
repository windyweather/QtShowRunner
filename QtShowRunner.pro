#-------------------------------------------------
#
# Project created by QtCreator 2020-04-06T15:20:06
#
#-------------------------------------------------

QT       += core gui
Q_OS_WINDOWS: LIBS += -lUser32
#QT += x11extras
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# not needed since X11 does not work
#LIBS += -L"/usr/lib/"
#LIBS += -lX11

TARGET = QtShowRunner
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    aboutdialog.cpp \
    helpdialog.cpp \
    mouseclick.cpp \
    xenumeratewindows.cpp

HEADERS += \
        mainwindow.h \
    aboutdialog.h \
    helpdialog.h \
    mouseclick.h

FORMS += \
        mainwindow.ui \
    aboutdialog.ui \
    helpdialog.ui
