/****************************************************************************
**
** Jreen
**
** Copyright © 2013 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "tlsfeature_p.h"
#include "client.h"
#include "logger.h"
#include "directconnection.h"
#include <QXmlStreamWriter>
#include <QCoreApplication>
#include <QSslSocket>

#define NS_TLS QLatin1String("urn:ietf:params:xml:ns:xmpp-tls")

namespace Jreen
{

TLSFeature::TLSFeature() : StreamFeature(SecurityLayer)
{
	m_required = false;
	m_available = false;
	if (!QSslSocket::supportsSsl())
		Logger::warning() << "Jreen: TLS is not provided by QtNetwork";
}

void TLSFeature::init()
{
}

void TLSFeature::setStreamInfo(StreamInfo *info)
{
	if(m_client)
		m_client->disconnect(this);
	StreamFeature::setStreamInfo(info);
	connect(m_client, SIGNAL(disconnected(Jreen::Client::DisconnectReason)),
	        SLOT(onDisconnected()));
}

void TLSFeature::reset()
{
	m_required = false;
	m_available = false;
}

bool TLSFeature::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(name);
	Q_UNUSED(attributes);
	// All other methods shouldn't be called is canParse returnes false
	if (!QSslSocket::supportsSsl())
		return false;

	if (DirectConnection *connection = qobject_cast<DirectConnection*>(m_client->connection())) {
		if (qobject_cast<QSslSocket*>(connection->socket()))
			return uri == NS_TLS;
	}
	return false;
}

void TLSFeature::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	if (name == QLatin1String("starttls"))
		m_available = true;
	else if (name == QLatin1String("required"))
		m_required = true;
}

void TLSFeature::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(uri);
	if (name == QLatin1String("proceed")) {
		DirectConnection *connection = qobject_cast<DirectConnection*>(m_client->connection());
		Q_ASSERT(connection);
		QSslSocket *socket = qobject_cast<QSslSocket*>(connection->socket());
		Q_ASSERT(socket);
		m_socket = socket;

		socket->setProtocol(QSsl::TlsV1);
		socket->setPeerVerifyMode(QSslSocket::VerifyNone);

		connect(socket, SIGNAL(encrypted()), this, SLOT(onHandshaken()));
		connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
		connect(socket, SIGNAL(peerVerifyError(QSslError)), SLOT(onPeerVerifyError(QSslError)));

		socket->startClientEncryption();
	}
}

void TLSFeature::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

bool TLSFeature::isActivatable()
{
	return QSslSocket::supportsSsl() && m_available;
}

bool TLSFeature::activate()
{
	init();
	QXmlStreamWriter *writer = m_info->writer();
	writer->writeEmptyElement(QLatin1String("starttls"));
	writer->writeDefaultNamespace(NS_TLS);
	writer->writeCharacters(QString());
	return true;
}

void TLSFeature::onHandshaken()
{
	m_info->completed();
}

void TLSFeature::onPeerVerifyError(const QSslError &error)
{
	QSslSocket *socket = qobject_cast<QSslSocket*>(sender());
	Q_ASSERT(socket);
	socket->ignoreSslErrors(QList<QSslError>() << error);
}

void TLSFeature::onDisconnected()
{
	if (m_socket)
		disconnect(m_socket, 0, this, 0);
	m_socket = NULL;
}

} //namespace Jreen


