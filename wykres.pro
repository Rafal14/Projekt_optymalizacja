#-------------------------------------------------
#
# Project created by QtCreator 2018-03-17T15:32:21
#
#-------------------------------------------------

QT       += core gui
CONFIG   += qwt

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = wykres
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
    simplextab.cpp \
    rysunek.cpp

HEADERS += \
        mainwindow.h \
    simplextab.h \
    rysunek.h

FORMS += \
        mainwindow.ui \
    rysunek.ui

CONFIG += static


win32: LIBS += -L$$PWD/../../../../../qwt/qwt_static/lib/ -lqwt

INCLUDEPATH += $$PWD/../../../../../qwt/qwt_static/include
DEPENDPATH += $$PWD/../../../../../qwt/qwt_static/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../../../../../qwt/qwt_static/lib/qwt.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../../../../../qwt/qwt_static/lib/libqwt.a
