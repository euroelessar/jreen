/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef CLIENT_P_H
#define CLIENT_P_H
#include <QDebug>
#include <QXmlStreamWriter>

#include "client.h"
#include "jid.h"
#include "parser.h"
#include "directconnection.h"
#include "streamfeature_p.h"
#include "iq.h"
#include "iqreply.h"
#include "message.h"
#include "presence.h"
#include "subscription.h"
#include "datastream.h"
#include "jstrings.h"
#include "disco.h"
#include "error.h"
#include "stanzafactory_p.h"
#include "buffereddatastream.h"
#include <QTimer>
#include <QTextCodec>
#include <QBuffer>
#include <QNetworkProxyFactory>
#include "stanza_p.h"
#include "streamprocessor.h"

namespace Jreen
{

class ClientPrivate;
class MUCRoomPrivate;

class IQTrack : public IQReply
{
	Q_OBJECT
	friend class ClientPrivate;
public:
	inline IQTrack(QObject *handler, const char *member, int ctx, Client *client)
	    : IQReply(client), context(ctx)
	{
		connect(this, SIGNAL(received(Jreen::IQ)), SLOT(onReceived(Jreen::IQ)));
		connect(this, SIGNAL(iqReceived(Jreen::IQ,int)), handler, member);
	}
	
public slots:
	void onReceived(const Jreen::IQ &iq)
	{
		emit iqReceived(iq, context);
	}

signals:
	void iqReceived(const Jreen::IQ &iq, int context);
	
private:
	int context;
};

class PresenceTrack : public QObject
{
	Q_OBJECT
	friend class ClientPrivate;
public:
	PresenceTrack(QObject *handler, const char *member) : QObject(handler)
	{
		connect(this, SIGNAL(presenceReceived(Jreen::Presence)), handler, member);
	}
	virtual ~PresenceTrack()
	{
	}
signals:
	void presenceReceived(const Jreen::Presence &presence);
};

class MessageTrack : public QObject
{
	Q_OBJECT
	friend class ClientPrivate;
public:
	MessageTrack(QObject *handler, const char *member) : QObject(handler)
	{
		connect(this, SIGNAL(messageReceived(Jreen::Message)), handler, member);
	}
	virtual ~MessageTrack()
	{
	}
signals:
	void messageReceived(const Jreen::Message &presence);
};

class ClientPrivate
{
	Q_DECLARE_PUBLIC(Client)
public:
	static ClientPrivate *get(Client *client) { return client->d_func(); }
	
	ClientPrivate(const Presence &p, Client *parent)
		:  pingInterval(-1), q_ptr(parent), presence(p), current_id(0), conn(0)
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
	int pingInterval;
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
	QHash<QString, IQReply*> iqTracks;
	QXmlStreamWriter *writer;
	QList<StanzaFactory*> stanzas;
	QList<StreamFeature*> features;
	QSet<QString> serverFeatures;
	Jreen::Disco::IdentityList serverIdentities;
	QMap<QString, MUCRoomPrivate*> rooms;
	PayloadFactoryMap factories;
	QMultiMap<QString, AbstractPayloadFactory*> factoriesByUri;
	MessageSessionManager *messageSessionManager;
	AbstractRoster *roster;
	QWeakPointer<QNetworkProxyFactory> proxyFactory;
	int depth;
	IQReply *createIQReply() { return new IQReply(q_func()); }
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
		pingTimer.stop();
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
		QHash<QString, IQReply*>::iterator it = iqTracks.begin();
		for (; it != iqTracks.end(); ++it)
			it.value()->deleteLater();
		iqTracks.clear();
	}
	inline void emitAuthorized() { q_ptr->handleAuthorized(); }
	inline void emitConnected() { isConnected = true; q_ptr->handleConnect(); }
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
