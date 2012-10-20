/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "connectionbosh.h"
#include "util.h"
#include "jid.h"
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include "logger.h"
#include <QBuffer>
#include <QPointer>
#include <QNetworkProxy>

namespace Jreen
{
class ConnectionBOSHPrivate
{
public:
	ConnectionBOSHPrivate() : resultBuffer(&resultXml), writer(&resultBuffer) {}
	quint64 rid;
	QString sessionId;
	QList<QByteArray> keys;
	int keyNum;
	JID jid;
	bool authorized;
	XmlStreamParser *streamParser;
	QNetworkAccessManager manager;
	QUrl host;
	QPointer<QNetworkReply> emptyRequest;
	QPointer<QNetworkReply> dataRequest;
	QByteArray payloads;
	QByteArray resultXml;
	QBuffer resultBuffer;
	QXmlStreamWriter writer;
	QXmlStreamReader reader;
	bool streamInitiation;
	int depth;
	
	QByteArray generateKey()
	{
		QByteArray seed = Util::randomHash();
		keys.clear();
		Logger::debug() << Q_FUNC_INFO << keyNum;
		for (int i = 0; i < keyNum; i++) {
			seed = QCryptographicHash::hash(seed, QCryptographicHash::Sha1).toHex();
			keys.append(seed);
		}
		return QCryptographicHash::hash(seed, QCryptographicHash::Sha1).toHex();
	}
	QByteArray nextKey()
	{
		return keys.takeLast();
	}
	void send(bool empty, bool header = false);
	void sendHeader(bool first);
};

void ConnectionBOSHPrivate::send(bool empty, bool header)
{
	QByteArray data = resultXml;
	resultBuffer.seek(0);
	resultXml.clear();
	Logger::debug() << Q_FUNC_INFO << data;
	QNetworkRequest request(host);
	request.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
	request.setHeader(QNetworkRequest::ContentTypeHeader, QByteArray("text/xml; charset=utf-8"));
	request.setRawHeader("Accept-Encoding", "gzip, deflate");
	QNetworkReply *reply = manager.post(request, data);
	(empty ? emptyRequest : dataRequest) = reply;
	reply->setProperty("header", header);
}

void ConnectionBOSHPrivate::sendHeader(bool first)
{
	streamInitiation = first;
	writer.writeEmptyElement(QLatin1String("body"));
	writer.writeAttribute(QLatin1String("rid"), QString::number(rid++));
	if (first) {
		writer.writeAttribute(QLatin1String("from"), jid.bare());
		writer.writeAttribute(QLatin1String("route"),
							  QLatin1String("xmpp:") + jid.domain() + QLatin1String(":5222"));
		writer.writeAttribute(QLatin1String("secure"), QLatin1String("true"));
		writer.writeAttribute(QLatin1String("hold"), QLatin1String("2"));
		writer.writeAttribute(QLatin1String("ver"), QLatin1String("1.8"));
		writer.writeAttribute(QLatin1String("xmpp:version"), QLatin1String("1.0"));
		writer.writeAttribute(QLatin1String("xml:lang"), QLatin1String("en"));
	} else {
		writer.writeAttribute(QLatin1String("xmpp:restart"), QLatin1String("true"));
		writer.writeAttribute(QLatin1String("key"), QLatin1String(nextKey()));
		writer.writeAttribute(QLatin1String("sid"), sessionId);
	}
	writer.writeAttribute(QLatin1String("newkey"), QLatin1String(generateKey()));
	writer.writeAttribute(QLatin1String("to"), jid.domain());
	writer.writeDefaultNamespace(QLatin1String("http://jabber.org/protocol/httpbind"));
	writer.writeAttribute(QLatin1String("xmlns:xmpp"), QLatin1String("urn:xmpp:xbosh"));
	writer.writeCharacters(QLatin1String(""));
	send(false, true);
}

ConnectionBOSH::ConnectionBOSH(const QString &host, int port) : d_ptr(new ConnectionBOSHPrivate)
{
	Q_D(ConnectionBOSH);
	d->streamInitiation = false;
	d->rid = 0;
	d->keyNum = 20;
	d->depth = 0;
	d->streamParser = 0;
	d->host.setScheme(QLatin1String("http"));
	d->host.setHost(host);
	d->host.setPort(port);
	d->host.setPath(QLatin1String("http-bind"));
	d->resultBuffer.open(QIODevice::ReadWrite);
	d->writer.writeStartDocument();
	d->writer.writeStartElement(QLatin1String("stream"));
	d->writer.writeCharacters(QLatin1String(""));
	d->reader.addData(d->resultXml);
	while (d->reader.readNext() > QXmlStreamReader::Invalid);
	d->resultBuffer.seek(0);
	d->resultXml.clear();
//	QNetworkProxy proxy;
//	proxy.setType(QNetworkProxy::HttpProxy);
//	proxy.setHostName("proxy.istu.ru");
//	proxy.setPort(8080);
//	d->manager.setProxy(proxy);
	connect(&d->manager, SIGNAL(finished(QNetworkReply*)), SLOT(onRequestFinished(QNetworkReply*)));
}

ConnectionBOSH::~ConnectionBOSH()
{
}

bool ConnectionBOSH::open()
{
	Q_D(ConnectionBOSH);
	d->rid = (quint64(qrand()) << 20) ^ quint64(qrand());
	d->keyNum = (qAbs(qrand()) % 30) + 20;
	d->sendHeader(true);
	d->authorized = false;
	d->depth = 0;
	QIODevice::open(QIODevice::ReadWrite);
	return true;
}

void ConnectionBOSH::close()
{
}

qint64 ConnectionBOSH::bytesAvailable() const
{
	return 0;
}

ConnectionBOSH::SocketState ConnectionBOSH::socketState() const
{
	return ConnectedState;
}

ConnectionBOSH::SocketError ConnectionBOSH::socketError() const
{
	return UnknownSocketError;
}

void ConnectionBOSH::authorized()
{
	Q_D(ConnectionBOSH);
	d->authorized = true;
	char c;
	writeData(&c, 0);
}

QString ConnectionBOSH::sessionID() const
{
	return d_func()->sessionId;
}

void ConnectionBOSH::setJID(const JID &jid)
{
	d_func()->jid = jid;
}

void ConnectionBOSH::setStreamParser(XmlStreamParser *parser)
{
	d_func()->streamParser = parser;
}

void ConnectionBOSH::restartStream()
{
	Q_D(ConnectionBOSH);
	d->sendHeader(false);
}

qint64 ConnectionBOSH::readData(char *data, qint64 maxlen)
{
	Q_UNUSED(data);
	Q_UNUSED(maxlen);
	return 0;
}

qint64 ConnectionBOSH::writeData(const char *payloaddata, qint64 payloadlen)
{
	Logger::debug() << Q_FUNC_INFO;
	Q_D(ConnectionBOSH);
	Logger::debug() << d->dataRequest << d->emptyRequest;
	if (d->dataRequest && payloadlen > 0) {
		d->payloads.append(payloaddata, payloadlen);
		return payloadlen;
	}
	bool isEmpty = payloadlen == 0;
	d->writer.writeStartElement(QLatin1String("body"));
	d->writer.writeAttribute(QLatin1String("rid"), QString::number(d->rid++));
	d->writer.writeAttribute(QLatin1String("sid"), d->sessionId);
	d->writer.writeAttribute(QLatin1String("key"), QLatin1String(d->nextKey()));
	if (d->keys.size() == 1)
		d->writer.writeAttribute(QLatin1String("newkey"), QLatin1String(d->generateKey()));
	d->writer.writeDefaultNamespace(QLatin1String("http://jabber.org/protocol/httpbind"));
	if (isEmpty)  {
	} else {
		d->writer.writeCharacters(QLatin1String(""));
		d->resultXml.append(payloaddata, payloadlen);
		d->resultBuffer.seek(d->resultXml.size());
	}
	d->writer.writeEndElement();
	d->send(isEmpty);
	return payloadlen;
}

void ConnectionBOSH::onRequestFinished(QNetworkReply *reply)
{
	Q_D(ConnectionBOSH);
	reply->deleteLater();
	Logger::debug() << Q_FUNC_INFO << reply->error() << reply->errorString();
	if (reply->error() != QNetworkReply::NoError) {
		// TODO: Implement
		return;
	}
	bool header = reply->property("header").toBool();
	QByteArray data = reply->readAll();
	Logger::debug() << Q_FUNC_INFO << header << data;
	d->reader.addData(data);
	// Hook for parsers invoked in eventloops, which are run inside parser
	if (d->depth != 0)
		return;
	while (d->reader.readNext() > QXmlStreamReader::Invalid) {
		switch(d->reader.tokenType()) {
		case QXmlStreamReader::StartElement:
			d->depth++;
			if (d->depth > 1) {
				d->streamParser->handleStartElement(d->reader.name(), d->reader.namespaceUri(),
													d->reader.attributes());
			} else {
				Q_ASSERT(d->reader.name() == QLatin1String("body"));
				const QXmlStreamAttributes attributes = d->reader.attributes();
				if (d->streamInitiation) {
					d->streamInitiation = false;
					d->sessionId = attributes.value(QLatin1String("sid")).toString();
					emit connected();
				}
				if (header)
					d->streamParser->handleStartElement(QStringRef(), QStringRef(),
														QXmlStreamAttributes());
			}
			break;
		case QXmlStreamReader::EndElement:
			if (d->depth > 1)
				d->streamParser->handleEndElement(d->reader.name(), d->reader.namespaceUri());
			d->depth--;
			break;
		case QXmlStreamReader::Characters:
			if (d->depth > 1)
				d->streamParser->handleCharacterData(d->reader.text());
			break;
		default:
			break;
		}
	}
	Q_ASSERT(d->depth == 0);
	if (!d->payloads.isEmpty() && reply == d->dataRequest) {
		d->dataRequest = 0;
		writeData(d->payloads.constData(), d->payloads.size());
		d->payloads.clear();
		return;
	} else if (reply == d->dataRequest) {
		d->dataRequest = 0;
	}
	if (reply == d->emptyRequest)
		d->emptyRequest = 0;
	if (d->authorized && !d->emptyRequest) {
		char c;
		writeData(&c, 0);
	}
}
}
