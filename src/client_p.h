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
#include <QBuffer>
#include "stanza_p.h"
#include "streamprocessor.h"

namespace Jreen
{

class ClientPrivate;
class MUCRoomPrivate;

class IQTrack : public QObject
{
	Q_OBJECT
	friend class ClientPrivate;
public:
	inline IQTrack(QObject *handler, const char *member, int ctx) : context(ctx)
	{
		connect(this, SIGNAL(newIQ(Jreen::IQ,int)), handler, member);
	}
	int context;
signals:
	void newIQ(const Jreen::IQ &iq, int context);
};

class PresenceTrack : public QObject
{
	Q_OBJECT
	friend class ClientPrivate;
public:
	PresenceTrack(QObject *handler, const char *member) : QObject(handler)
	{
		connect(this, SIGNAL(newPresence(Jreen::Presence)), handler, member);
	}
	virtual ~PresenceTrack()
	{
	}
signals:
	void newPresence(const Jreen::Presence &presence);
};

class MessageTrack : public QObject
{
	Q_OBJECT
	friend class ClientPrivate;
public:
	MessageTrack(QObject *handler, const char *member) : QObject(handler)
	{
		connect(this, SIGNAL(newMessage(Jreen::Message)), handler, member);
	}
	virtual ~MessageTrack()
	{
	}
signals:
	void newMessage(const Jreen::Message &presence);
};

class ClientPrivate
{
	Q_DECLARE_PUBLIC(Client)
public:
	static ClientPrivate *get(Client *client) { return client->d_func(); }
	
	ClientPrivate(const Presence &p, Client *parent)
		:  q_ptr(parent),presence(p), current_id(0), conn(0)
	{
		Q_Q(Client);
		disco = 0;
		current_stream_feature = 0;
		messageSessionManager = 0;
		roster = 0;
		authorized = false;
		isConnected = false;
		device = new BufferedDataStream(&streamHandlers);
		device->open(QIODevice::ReadWrite);
		q->connect(device, SIGNAL(readyRead()), q, SLOT(_q_new_data()));
	}
	void init();
	void send(const Stanza &stanza)
	{
		if(stanza.from().full().isEmpty()) {
			const StanzaPrivate *p = StanzaPrivate::get(stanza);
			const_cast<StanzaPrivate*>(p)->from = jid;
		}
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
	QBasicTimer pingTimer;
	StreamInfo *stream_info;
	Client *q_ptr;
	JID jid;
	QString sid;
	QString server;
	QString password;
	int server_port;
	QList<XmlStreamHandler*> streamHandlers;
	Presence presence;
	int current_id;
	Parser *parser;
	Connection *conn;
	DataStream *device;
	StreamProcessor *streamProcessor;
	QList<DataStream*> devices;
	bool authorized;
	bool isConnected;
	// And again compression
	Disco *disco;
	StreamFeature *current_stream_feature;
	QHash<QString,IQTrack *> iq_tracks;
	QXmlStreamWriter *writer;
	QList<StanzaFactory*> stanzas;
	QList<StreamFeature*> features;
	QSet<QString> serverFeatures;
	Jreen::Disco::IdentityList serverIdentities;
	QMap<QString, MUCRoomPrivate*> rooms;
	StanzaExtensionFactoryMap factories;
	QMultiMap<QString, AbstractStanzaExtensionFactory*> factoriesByUri;
	MessageSessionManager *messageSessionManager;
	AbstractRoster *roster;
	int depth;
	void _q_iq_received(const Jreen::IQ &iq, int context);
	void _q_new_data()
	{
		QByteArray data = device->read(qMax(Q_INT64_C(0xffff), device->bytesAvailable())); // device->readAll();
		//		qDebug() << "-" << data.size() << data;
		parser->appendData(data);
		//		parser->appendData(data);
		_q_read_more();
	}
	void _q_read_more();
	void _q_send_header()
	{
		delete writer;
		foreach (XmlStreamHandler *handler, streamHandlers)
			handler->handleStreamBegin();
		if (streamProcessor) {
			writer = new QXmlStreamWriter(device);
//			QByteArray data;
//			QBuffer buffer(&data);
//			buffer.open(QIODevice::WriteOnly);
//			streamProcessor->restartStream();
//			writer = new QXmlStreamWriter(&buffer);
//			writer->writeStartDocument(QLatin1String("1.0"));
//			writer->writeStartElement(QLatin1String("stream:stream"));
//			writer->writeDefaultNamespace(QLatin1String("jabber:client"));
//			writer->writeCharacters(QString());
//			writer->setDevice(device);
			return;
		}
		writer = new QXmlStreamWriter(device);
		writer->writeStartDocument(QLatin1String("1.0"));
		writer->writeStartElement(QLatin1String("stream:stream"));
		writer->writeAttribute(QLatin1String("to"), jid.domain());
		writer->writeDefaultNamespace(QLatin1String("jabber:client"));
		writer->writeAttribute(QLatin1String("xmlns:stream"), QLatin1String("http://etherx.jabber.org/streams"));
		writer->writeAttribute(QLatin1String("xml:lang"), QLatin1String("en"));
		writer->writeAttribute(QLatin1String("version"), QLatin1String("1.0"));
		writer->writeCharacters(QString());
	}

	void _q_connected()
	{
		writer = 0;
		depth = 0;
		parser->reset();
		_q_send_header();
		isConnected = true;
	}
	void _q_disconnected()
	{
		isConnected = false;
		foreach (XmlStreamHandler *handler, streamHandlers)
			handler->handleStreamEnd();
		authorized = false;
		current_stream_feature = 0;
		presence.setSubtype(Presence::Unavailable);
		q_ptr->handleDisconnect();
		foreach (DataStream *dataStream, devices)
			dataStream->deleteLater();
		devices.clear();
		device->setDevice(conn);
	}
	inline void emitAuthorized() { q_ptr->handleAuthorized(); }
	inline void emitConnected() { q_ptr->handleConnect(); }
	inline void emitDisconnected(Client::DisconnectReason reason)
	{
		emit q_func()->disconnected(reason);
	}
};

class StreamInfoImpl : public StreamInfo
{
public:
	StreamInfoImpl(ClientPrivate *clientPrivate)
	{
		d = clientPrivate;
	}
	QString streamID()
	{
		//		if(!m_client_private->current_stream_feature)
		//			return QString();
		if (d->streamProcessor)
			return d->streamProcessor->sessionID();
		return d->sid;
	}
	QString connectionServer()
	{
		//		if(!m_client_private->current_stream_feature)
		//			return QString();
		return d->server;
	}
	JID jid()
	{
		//		if(!m_client_private->current_stream_feature)
		//			return JID();
		return d->jid;
	}
	QString password()
	{
		//		if(!m_client_private->current_stream_feature
		//			|| (m_client_private->current_stream_feature->type() != StreamFeature::SASL
		//			&& m_client_private->current_stream_feature->type() != StreamFeature::SimpleAuthorization))
		//			return QString();
		return d->password;
	}
	Client *client()
	{
		return d->q_ptr;
	}
	QXmlStreamWriter *writer()
	{
		return d->writer;
	}
	void completed(const CompletedFlags &flags)
	{
		if(flags & AuthorizationFailed) {
			emit d->emitDisconnected(Client::AuthorizationError);
			return;
		}
		if(flags & Authorized)
			d->emitAuthorized();
		if (flags & ResendHeader) {
			d->device->readAll();
			d->_q_send_header();
			if (d->streamProcessor)
				d->streamProcessor->restartStream();
			d->parser->reset();
			d->current_stream_feature = 0;
		}
		if (flags & ActivateNext)
			d->parser->activateFeature();
		if (flags & Connected) {
			d->emitConnected();
			if (d->streamProcessor)
				d->streamProcessor->authorized();
		}
	}
	void setJID(const JID &jid)
	{
		const StanzaPrivate *p = StanzaPrivate::get(d->presence);
		const_cast<StanzaPrivate*>(p)->from = jid;
		d->jid = jid;
	}
	void addDataStream(DataStream *dataStream) 
	{
		d->devices.append(dataStream);
		dataStream->setDevice(d->device->device());
		d->device->setDevice(dataStream);
		//		QObject::disconnect(m_client_private->device, 0, m_client_private, 0);
		//		m_client_private->device = dataStream;
		dataStream->open(QIODevice::ReadWrite);
		//		QObject::connect(m_client_private->device, SIGNAL(readyRead()), m_client_private, SLOT(newData()));
	}
private:
	ClientPrivate *d;
};

}

#endif // CLIENT_P_H
