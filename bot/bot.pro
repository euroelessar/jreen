# -------------------------------------------------
# Project created by QtCreator 2009-04-16T23:32:06
# -------------------------------------------------
QT += xml \
    testlib
# QT -= gui
LIBS += -L../ -ljreen
INCLUDEPATH += ../src
TARGET = bot
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += main.cpp \
    bot.cpp
HEADERS += bot.h
