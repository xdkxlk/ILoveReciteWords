#-------------------------------------------------
#
# Project created by QtCreator 2014-11-28T21:27:44
#
#-------------------------------------------------

QT       += core gui
    INCLUDEPATH += "C:/Program Files/Microsoft Speech SDK 5.1/Include"

    LIBS += -L"C:/Program Files/Microsoft Speech SDK 5.1/Lib/i386"

QT+=axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WordStudy
TEMPLATE = app


SOURCES += \
    cpeople.cpp \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    cwordlist.cpp \
    cword.cpp \
    cwordlistnewword.cpp \
    cwordstulist.cpp \
    nmlistwidget.cpp \
    addnwlistdialog.cpp \
    cwordlistrev.cpp \
    cspeak.cpp

HEADERS  += \
    cpeople.h \
    logindialog.h \
    mainwindow.h \
    cwordlist.h \
    cword.h \
    cwordlistnewword.h \
    cwordstulist.h \
    nmlistwidget.h \
    addnwlistdialog.h \
    cwordlistrev.h \
    cspeak.h

FORMS    += \
    logindialog.ui \
    mainwindow.ui \
    addnwlistdialog.ui
