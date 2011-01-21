/****************************************************************************
 *  directconnection.cpp
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

#include "directconnection_p.h"

namespace jreen
{

DirectConnection::DirectConnection(QAbstractSocket *socket, const QString &host_name, quint16 port)
	: d_ptr(new DirectConnectionPrivate(host_name, port, this))
{
	Q_ASSERT(socket);
	d_ptr->socket = socket;
	d_ptr->connectSocket();
}

DirectConnection::DirectConnection(QAbstractSocket *socket, const QHostAddress &address, quint16 port)
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
	}
	else
		d->socket->connectToHost(d->host_name, d->port);
	return true;
}

void DirectConnection::close()
{
	Q_D(DirectConnection);
	d->socket->flush();
	d->socket->disconnectFromHost();
	QIODevice::close();
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
