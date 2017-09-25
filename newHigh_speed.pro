#-------------------------------------------------
#
# Project created by QtCreator 2017-09-25T14:43:55
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = newHigh_speed
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
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
    imageprocthread.cpp \
    handpiecimage.cpp

HEADERS += \
        mainwindow.h \
    imageprocthread.h \
    handpiecimage.h

FORMS += \
        mainwindow.ui \
    handpiecimage.ui

win32: LIBS += -L$$PWD/'../../../../Program Files/Python36/libs/' -lpython36
INCLUDEPATH += $$PWD/'../../../../Program Files/Python36/libs'
INCLUDEPATH += $$PWD/'../../../../Program Files/Python36/include'
DEPENDPATH += $$PWD/'../../../../Program Files/Python36/libs'

DISTFILES += \
    test.py
