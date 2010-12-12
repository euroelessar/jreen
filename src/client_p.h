/****************************************************************************
 *  client_p.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef CLIENT_P_H
#define CLIENT_P_H
#include <QDebug>
#include <QXmlStreamWriter>

#include "client.h"
#include "jid.h"
#include "parser.h"
#include "directconnection.h"
#include "streamfeature.h"
#include "iq.h"
#include "message.h"
#include "presence.h"
#include "subscription.h"
#include "datastream.h"
#include "jstrings.h"
#include "disco.h"
#include "error.h"
#include "stanzafactory.h"
#include "buffereddatastream.h"
#include <QTimer>
#include <QTextCodec>
#include "stanza_p.h"

namespace jreen
{

class ClientPrivate;

class IQTrack : public QObject
{
	Q_OBJECT
	friend class ClientPrivate;
public:
	inline IQTrack(QObject *handler, const char *member, int ctx) : context(ctx)
	{
		connect(this, SIGNAL(newIQ(jreen::IQ,int)), handler, member);
	}
	int context;
signals:
	void newIQ(const jreen::IQ &iq, int context);
};

class PresenceTrack : public QObject
{
	Q_OBJECT
	friend class ClientPrivate;
public:
	PresenceTrack(QObject *handler, const char *member) : QObject(handler)
	{
		connect(this, SIGNAL(newPresence(jreen::Presence)), handler, member);
	}
	virtual ~PresenceTrack()
	{
	}
signals:
	void newPresence(const jreen::Presence &presence);
};

class MessageTrack : public QObject
{
	Q_OBJECT
	friend class ClientPrivate;
public:
	MessageTrack(QObject *handler, const char *member) : QObject(handler)
	{
		connect(this, SIGNAL(newMessage(jreen::Message)), handler, member);
	}
	virtual ~MessageTrack()
	{
	}
signals:
	void newMessage(const jreen::Message &presence);
};

class ClientPrivate : public QObject
{
	Q_OBJECT
public:
	static ClientPrivate *get(Client *client) { return client->impl; }
	
	ClientPrivate(const Presence &p, Client *parent)
		: QObject(parent), presence(p), current_id(0), conn(0)
	{
		disco = 0;
		current_stream_feature = 0;
		authorized = false;
		client = parent;
		device = new BufferedDataStream;
		device->open(QIODevice::ReadWrite);
		connect(device, SIGNAL(readyRead()), this, SLOT(newData()));
	}
	void init();
	QString elementToString(const QDomElement &element)
	{
		static QTextCodec *utf = QTextCodec::codecForName("utf-8");
		QString xml;
		QTextStream *stream = new QTextStream(&xml, QIODevice::WriteOnly);
		stream->setCodec(utf);
		element.save(*stream, 0, QDomNode::EncodingFromTextStream);
		delete stream;
		qDebug("%s", qPrintable(xml));
		return xml;
	}
	void send(const Stanza &stanza)
	{
		foreach (StanzaFactory *factory, stanzas) {
			if (factory->stanzaType() == StanzaPrivate::get(stanza)->type) {
				factory->serialize(const_cast<Stanza*>(&stanza), writer);
				break;
			}
		}
	}
	void send(const QString &data)
	{
		if(conn && device->isOpen())
			device->write(data.toUtf8());
	}
	void processStreamFeature(StreamFeature *stream_feature)
	{
		current_stream_feature = stream_feature;
	}
	void registerPresenceHandler(const JID &jid, QObject *handler, const char *member)
	{
		Q_UNUSED(jid);
		Q_UNUSED(handler);
		Q_UNUSED(member);
	}
	//	void registerMessageHandler(const JID &jid, QObject *handler, const char *member);

	void handleStanza(const Stanza::Ptr &stanza);
	StreamInfo *stream_info;
	Client *client;
	JID jid;
	QString sid;
	QString server;
	QString password;
	int server_port;
	Presence presence;
	int current_id;
	Parser *parser;
	Connection *conn;
	DataStream *device;
	bool authorized;
	// And again compression
	Disco *disco;
	StreamFeature *current_stream_feature;
	QHash<QString,IQTrack *> iq_tracks;
	QXmlStreamWriter *writer;
	QList<StanzaFactory*> stanzas;
	QList<StreamFeature*> features;
	QSet<QString> serverFeatures;
	StanzaExtensionFactoryMap factories;
	int depth;
public slots:
	void onIqReceived(const jreen::IQ &iq, int context);
	void newData()
	{
		QByteArray data = device->read(qMax(Q_INT64_C(0xffff), device->bytesAvailable())); // device->readAll();
		//		qDebug() << "-" << data.size() << data;
		parser->appendData(data);
		//		parser->appendData(data);
		readMore();
	}
	void readMore();
	void sendHeader()
	{
		qDebug() << device;
		delete writer;
		qDebug() << device;
		writer = new QXmlStreamWriter(device);
		qDebug() << conn;
		writer->writeStartDocument(QLatin1String("1.0"));
		writer->writeStartElement(QLatin1String("stream:stream"));
		writer->writeAttribute(QLatin1String("to"), jid.domain());
		writer->writeDefaultNamespace(QLatin1String("jabber:client"));
		writer->writeAttribute(QLatin1String("xmlns:stream"), QLatin1String("http://etherx.jabber.org/streams"));
		writer->writeAttribute(QLatin1String("xml:lang"), QLatin1String("en"));
		writer->writeAttribute(QLatin1String("version"), QLatin1String("1.0"));
		writer->writeCharacters(QString());
	}
	
	void connected()
	{
		writer = 0;
		depth = 0;
		sendHeader();
		//		QString head = "<?xml version='1.0' ?>"
		//		"<stream:stream to='" + jid.domain() + "' xmlns='jabber:client' "
		//		"xmlns:stream='http://etherx.jabber.org/streams' xml:lang='" "en" "' "
		//		"version='1.0'>";
		//		conn->write(head.toUtf8());
		
		//	<stream:stream to='qutim.org' xmlns='jabber:client' xmlns:stream='http://etherx.jabber.org/streams' xml:lang='en' version='1.0'>
		//  <stream:stream to="qutim.org" xmlns="jabber:client" xmlns:stream="http://etherx.jabber.org/streams" xml:lang="en" version="1.0">
		//  <stream xmlns="jabber:client" to="qutim.org" xmlns:n1="stream" n1:xmlns="http://etherx.jabber.org/streams" xml:lang="en" version="1.0">
		//  <stream xmlns="stream" to="qutim.org" xmlns="http://etherx.jabber.org/streams" xml:lang="en" version="1.0">
		//  <stream xmlns="stream" to="qutim.org" xmlns="http://etherx.jabber.org/streams" xmlns:n1="xml" n1:lang="en" version="1.0"><!---->
		//  <stream xmlns:stream="jabber:client" to="qutim.org" xmlns="http://etherx.jabber.org/streams" xmlns:n1="xml" n1:lang="en" version="1.0">
		//		client->handleConnect();
	}
	void disconnected()
	{
		authorized = false;
		current_stream_feature = 0;
		presence.setSubtype(Presence::Unavailable);
		client->handleDisconnect();
	}
	inline void emitAuthorized() { client->handleAuthorized(); }
	inline void emitConnected() { client->handleConnect(); }
};

class StreamInfoImpl : public StreamInfo
{
public:
	StreamInfoImpl(ClientPrivate *client_private)
	{
		m_client_private = client_private;
	}
	QString streamID()
	{
		//		if(!m_client_private->current_stream_feature)
		//			return QString();
		return m_client_private->sid;
	}
	QString connectionServer()
	{
		//		if(!m_client_private->current_stream_feature)
		//			return QString();
		return m_client_private->server;
	}
	JID jid()
	{
		//		if(!m_client_private->current_stream_feature)
		//			return JID();
		return m_client_private->jid;
	}
	QString password()
	{
		//		if(!m_client_private->current_stream_feature
		//			|| (m_client_private->current_stream_feature->type() != StreamFeature::SASL
		//			&& m_client_private->current_stream_feature->type() != StreamFeature::SimpleAuthorization))
		//			return QString();
		return m_client_private->password;
	}
	Client *client()
	{
		return m_client_private->client;
	}
	QXmlStreamWriter *writer()
	{
		return m_client_private->writer;
	}
	void completed(const CompletedFlags &flags)
	{
		if(flags & Authorized)
			m_client_private->emitAuthorized();
		if (flags & ResendHeader) {
			m_client_private->device->readAll();
			m_client_private->sendHeader();
			m_client_private->parser->reset();
			m_client_private->current_stream_feature = 0;
		}
		if (flags & AcitvateNext)
			m_client_private->parser->activateFeature();
		if (flags & Connected)
			m_client_private->emitConnected();
	}
	void setJID(const JID &jid)
	{
		m_client_private->jid = jid;
	}
	void addDataStream(DataStream *dataStream) 
	{
		dataStream->setDevice(m_client_private->device->device());
		m_client_private->device->setDevice(dataStream);
		//		QObject::disconnect(m_client_private->device, 0, m_client_private, 0);
		//		m_client_private->device = dataStream;
		dataStream->open(QIODevice::ReadWrite);
		//		QObject::connect(m_client_private->device, SIGNAL(readyRead()), m_client_private, SLOT(newData()));
	}
private:
	ClientPrivate *m_client_private;
};

}

#endif // CLIENT_P_H
