/****************************************************************************
**
** Jreen
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#include "directconnection_p.h"
#include <QSslSocket>
#include <QSslConfiguration>
#include <QSsl>

namespace Jreen
{

DirectConnection::DirectConnection(QAbstractSocket *socket, const QString &host_name, qint16 port)
	: d_ptr(new DirectConnectionPrivate(host_name, port, this))
{
	Q_ASSERT(socket);
	d_ptr->socket = socket;
	d_ptr->connectSocket();
}

DirectConnection::DirectConnection(QAbstractSocket *socket, const QHostAddress &address, qint16 port)
	: d_ptr(new DirectConnectionPrivate(address.toString(), port, this))
{
	Q_ASSERT(socket);
	d_ptr->socket = socket;
	d_ptr->connectSocket();
}

DirectConnection::~DirectConnection()
{
	delete d_ptr->socket;
}

bool DirectConnection::open()
{
	Q_D(DirectConnection);
	if(d->socket_state != QAbstractSocket::UnconnectedState) {
		if(d->socket_state == QAbstractSocket::ListeningState) {
			d->socket_state = QAbstractSocket::ConnectedState;
			QIODevice::open(ReadWrite);
			emit stateChanged(static_cast<SocketState>(d->socket_state));
		}
		return true;
	}
	if(d->do_lookup) {
		d->doLookup();
	} else {
		if (QSslSocket *socket = qobject_cast<QSslSocket*>(d->socket)) {
			socket->setPeerVerifyMode(QSslSocket::VerifyNone);
			QSslConfiguration conf = socket->sslConfiguration();
			conf.setProtocol(QSsl::TlsV1);
			socket->setSslConfiguration(conf);
			socket->connectToHostEncrypted(d->host_name, d->port);
		} else {
			d->socket->connectToHost(d->host_name, d->port);
		}
	}
	return true;
}

void DirectConnection::close()
{
	Q_D(DirectConnection);
	d->socket->flush();
	d->socket->disconnectFromHost();
	QIODevice::close();
}

void DirectConnection::setHost(const QHostAddress &host)
{
	d_func()->host_name = host.toString();
}

void DirectConnection::setHost(const QString &host)
{
	d_func()->host_name = host;
}

void DirectConnection::setPort(int port)
{
	d_func()->port = port;
}

void DirectConnection::setProxy(const QNetworkProxy &proxy)
{
	d_func()->socket->setProxy(proxy);
}

qint64 DirectConnection::bytesAvailable() const
{
	return d_func()->socket->bytesAvailable() + QIODevice::bytesAvailable();
}

Connection::SocketState DirectConnection::socketState() const
{
	Q_D(const DirectConnection);
	return static_cast<SocketState>(d->socket_state);
}

Connection::SocketError DirectConnection::socketError() const
{
	Q_D(const DirectConnection);
	return static_cast<SocketError>(d->socket_error);
}

qint64 DirectConnection::readData(char *data, qint64 maxlen)
{
	Q_D(DirectConnection);
	return d->socket->read(data, maxlen);
}

qint64 DirectConnection::writeData(const char *data, qint64 len)
{
	Q_D(DirectConnection);
	return d->socket->write(data, len);
}

}
