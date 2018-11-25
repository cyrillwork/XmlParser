#-------------------------------------------------
#
# Project created by QtCreator 2018-11-21T12:15:31
#
#-------------------------------------------------

QT       += core gui xml sql

CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XmlParser
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    database.cpp \
    parserxmlworker.cpp \
    dialogeditrecord.cpp

HEADERS  += mainwindow.h \
    database.h \
    parserxmlworker.h \
    dialogeditrecord.h

FORMS    += mainwindow.ui \
    dialogeditrecord.ui
