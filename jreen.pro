# -------------------------------------------------
# Project created by QtCreator 2009-03-31T00:30:15
# -------------------------------------------------
QT += network \
    xml
QT -= gui
TARGET = jreen
#CONFIG += dll
#TEMPLATE = lib
#VERSION = 0.1.0

LIBS += /usr/lib/qca2/libqca.so

 CONFIG += console
# CONFIG -= app_bundle
 TEMPLATE = app
SOURCES += src/jid.cpp \
    src/util.cpp \
    src/prep.cpp \
    src/parser.cpp \
    src/stanza.cpp \
    src/iq.cpp \
    src/message.cpp \
    src/presence.cpp \
    src/subscription.cpp \
    src/client.cpp \
    src/directconnection.cpp \
    src/domcreater.cpp \
    src/sjdns.cpp \
    src/tcpconnection.cpp \
    src/udpconnection.cpp \
    src/action.cpp \
    src/streamfeature.cpp \
    src/datastream.cpp \
    src/nonsaslauth.cpp \
    src/xqueryelement.cpp \
    src/xquerycontainer.cpp \
    src/langmap.cpp \
    src/delayeddelivery.cpp \
    src/error.cpp \
    src/softwareversion.cpp \
    src/capabilities.cpp \
    src/disco.cpp \
    src/abstractroster.cpp \
    src/dataform.cpp \
    src/privatexml.cpp \
    src/messagesession.cpp \
    src/imagemessage.cpp \
    src/nickname.cpp \
    src/receipt.cpp \
    src/mucroom.cpp \
    src/stanzaextension.cpp \
    src/main.cpp \
    src/stanzafactory.cpp \
    src/iqfactory.cpp \
    src/saslfeature.cpp \
    src/bindfeature.cpp \
    src/sessionfeature.cpp
HEADERS += src/jid.h \
    src/jreen.h \
    src/jstrings.h \
    src/util.h \
    src/prep.h \
    src/stanzaextension.h \
    src/parser.h \
    src/parser_p.h \
    src/stanza.h \
    src/stanza_p.h \
    src/iq.h \
    src/message.h \
    src/presence.h \
    src/subscription.h \
    src/client.h \
    src/client_p.h \
    src/directconnection.h \
    src/directconnection_p.h \
    src/domcreater_p.h \
    src/connection.h \
    src/sjdns_p.h \
    src/tcpconnection.h \
    src/udpconnection.h \
    src/action.h \
    src/streamfeature.h \
    src/datastream.h \
    src/nonsaslauth.h \
    src/xqueryelement_p.h \
    src/xquerycontainer_p.h \
    src/langmap.h \
    src/delayeddelivery.h \
    src/error.h \
    src/softwareversion.h \
    src/capabilities.h \
    src/disco.h \
    src/abstractroster.h \
    src/dataform.h \
    src/privatexml.h \
    src/privatexml_p.h \
    src/messagesession.h \
    src/imagemessage.h \
    src/nickname.h \
    src/receipt.h \
    src/mucroom.h \
    src/stanzafactory.h \
    src/iqfactory_p.h \
    src/iq_p.h \
    src/saslfeature.h \
    src/bindfeature.h \
    src/sessionfeature_p.h
include(src/gui/gui.pri)
include(src/3rdparty/jdns/jdns.pri)
DEFINES += J_BUILD_LIBRARY \
    J_BUILD_STATIC
