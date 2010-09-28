DEPENDPATH += $$PWD/..
QT += gui
CONFIG += designer
HEADERS += $$PWD/jidedit.h \
    $$PWD/jidedit_p.h \
    $$PWD/xmlconsole.h
SOURCES += $$PWD/jidedit.cpp \
    $$PWD/xmlconsole.cpp
FORMS += $$PWD/xmlconsole.ui
