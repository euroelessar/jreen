/****************************************************************************
 *  client_p.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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
#include "xquerycontainer_p.h"
#include "iq.h"
#include "message.h"
#include "presence.h"
#include "subscription.h"
#include "datastream.h"
#include "jstrings.h"
#include "disco.h"
#include "error.h"
#include <QTimer>
#include <QTextCodec>

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
		connect(this, SIGNAL(newIQ(IQ,int)), handler, member);
	}
	int context;
signals:
	void newIQ(const IQ &iq, int context);
};

class PresenceTrack : public QObject
{
	Q_OBJECT
	friend class ClientPrivate;
public:
	PresenceTrack(QObject *handler, const char *member) : QObject(handler)
	{
		connect(this, SIGNAL(newPresence(Presence)), handler, member);
	}
	virtual ~PresenceTrack()
	{
	}
signals:
	void newPresence(const Presence &presence);
};

class MessageTrack : public QObject
{
	Q_OBJECT
	friend class ClientPrivate;
public:
	MessageTrack(QObject *handler, const char *member) : QObject(handler)
	{
		connect(this, SIGNAL(newMessage(Message)), handler, member);
	}
	virtual ~MessageTrack()
	{
	}
signals:
	void newMessage(const Message &presence);
};

class ClientPrivate : public QObject
{
	Q_OBJECT
public:
	ClientPrivate(const Presence &p, Client *parent)
		: QObject(parent), presence(p), current_id(0), parser(new Parser), conn(0)
	{
		disco = 0;
		current_stream_feature = 0;
		authorized = false;
		client = parent;
	}
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
		stanza.writeXml(writer);
	}
	void send(const QString &data)
	{
		if(conn && conn->isOpen())
			conn->write(data.toUtf8());
	}
	void processStreamFeature(StreamFeature *stream_feature, const QDomElement &node)
	{
		current_stream_feature = stream_feature;
//		stream_feature->processElement(node);
	}
	void registerPresenceHandler(const JID &jid, QObject *handler, const char *member)
	{
		Q_UNUSED(jid);
		Q_UNUSED(handler);
		Q_UNUSED(member);
	}
//	void registerMessageHandler(const JID &jid, QObject *handler, const char *member);

	void elementParsed(const QDomElement &node)
	{
		elementToString(node);
		static const QString stream_features_str("stream:features");
		if(current_stream_feature)
		{
			processStreamFeature(current_stream_feature, node);
		}
		else if(node.nodeName() == stream_features_str && node.namespaceURI() == ConstString::ns_etherx)
		{
			StreamFeature *stream = security_layers.findStreamFeature(node);
			if(!stream)
				stream = compressions.findStreamFeature(node);
			if(!stream)
				stream = sasl_auths.findStreamFeature(node);
			if(!stream)
				stream = compressions.findStreamFeature(node);
			if(!stream)
				stream = non_sasl_auths.findStreamFeature(node);
			if(stream)
				processStreamFeature(stream, node);
			return;
		}

		if(node.nodeName() == QLatin1String("iq"))
		{
			IQ stanza(node);
			xquery.parseElement(stanza, node);
			IQTrack *track = iq_tracks.take(stanza.id());
			if(track)
			{
				emit track->newIQ(stanza, track->context);
				delete track;
			}
			else
			{
				client->handleIQ(stanza);
				if(!stanza.accepted() && (stanza.subtype() == IQ::Set || stanza.subtype() == IQ::Get))
				{
					IQ error(IQ::Error, stanza.from(), stanza.id());
					error.addExtension(new Error(Error::Cancel, Error::ServiceUnavailable));
					send(error);
				}
			}
		}
		else if(node.nodeName() == QLatin1String("message"))
		{
			Message stanza(node);
			xquery.parseElement(stanza, node);
			client->handleMessage(stanza);
		}
		else if(node.nodeName() == QLatin1String("presence"))
		{
			QString type = node.attribute(ConstString::type);
			if(type == QLatin1String("subscribe")  || type == QLatin1String("unsubscribe")
				|| type == QLatin1String("subscribed") || type == QLatin1String("unsubscribed"))
			{
				Subscription stanza(node);
				xquery.parseElement(stanza, node);
				client->handleSubscription(stanza);
			}
			else
			{
				Presence stanza(node);
				xquery.parseElement(stanza, node);
				client->handlePresence(stanza);
			}
		}
	}
	StreamInfo *stream_info;
	Client *client;
	JID jid;
	XQueryContainer xquery;
	QString sid;
	QString server;
	QString password;
	int server_port;
	Presence presence;
	int current_id;
	Parser *parser;
	Connection *conn;
	bool authorized;
	XQueryContainer security_layers;
	XQueryContainer compressions;
	XQueryContainer sasl_auths;
	// And again compression
	XQueryContainer non_sasl_auths;
	Disco *disco;
	StreamFeature *current_stream_feature;
	QHash<QString,IQTrack *> iq_tracks;
	QXmlStreamWriter *writer;
	QXmlStreamReader *reader;
	QList<StreamFeature*> streamFeatures;
	int depth;
public slots:
	void newData()
	{
		QByteArray data = conn->readAll();
		reader->addData(data);
//		parser->appendData(data);
		readMore();
	}
	void readMore();
	void connected()
	{
		writer = new QXmlStreamWriter(conn);
		reader = new QXmlStreamReader();
		depth = 0;
		writer->writeStartDocument(QLatin1String("1.0"));
		writer->writeStartElement(QLatin1String("stream:stream"));
		writer->writeAttribute("to", jid.domain());
		writer->writeDefaultNamespace("jabber:client");
		writer->writeAttribute("xmlns:stream", "http://etherx.jabber.org/streams");
		writer->writeAttribute("xml:lang", "en");
		writer->writeAttribute("version", "1.0");
		writer->writeCharacters(QString());
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
		client->handleConnect();
	}
	void disconnected()
	{
		non_sasl_auths.resetFeatures();
		sasl_auths.resetFeatures();
		compressions.resetFeatures();
		security_layers.resetFeatures();
		authorized = false;
		current_stream_feature = 0;
		presence.setPresence(Presence::Unavailable);
		client->handleDisconnect();
	}
	inline void emitAuthorized() { client->handleAuthorized(); }
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
		if(!m_client_private->current_stream_feature)
			return QString();
		return m_client_private->sid;
	}
	QString connectionServer()
	{
		if(!m_client_private->current_stream_feature)
			return QString();
		return m_client_private->server;
	}
	JID jid()
	{
		if(!m_client_private->current_stream_feature)
			return JID();
		return m_client_private->jid;
	}
	QString password()
	{
		if(!m_client_private->current_stream_feature
			|| (m_client_private->current_stream_feature->type() != StreamFeature::SASL
			&& m_client_private->current_stream_feature->type() != StreamFeature::SimpleAuthorization))
			return QString();
		return m_client_private->password;
	}
	Client *client()
	{
		return m_client_private->client;
	}
	void completed()
	{
		if(m_client_private->current_stream_feature->type() == StreamFeature::SASL
			|| m_client_private->current_stream_feature->type() == StreamFeature::SimpleAuthorization)
			m_client_private->emitAuthorized();
		m_client_private->current_stream_feature = 0;
	}
	void addDataStream(DataStream *data_stream) { Q_UNUSED(data_stream); }
private:
	ClientPrivate *m_client_private;
};

}

#endif // CLIENT_P_H
