/****************************************************************************
 *  directconnection.cpp
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

#include "directconnection_p.h"

J_BEGIN_NAMESPACE

DirectConnection::DirectConnection( QAbstractSocket *socket, const QString &host_name, quint16 port )
		: j_ptr(new DirectConnectionPrivate(host_name, port, this))
{
	Q_ASSERT( socket );
	j_ptr->socket = socket;
	j_ptr->connectSocket();
}

DirectConnection::DirectConnection( QAbstractSocket *socket, const QHostAddress &address, quint16 port )
		: j_ptr(new DirectConnectionPrivate(address.toString(), port, this))
{
	Q_ASSERT( socket );
	j_ptr->socket = socket;
	j_ptr->connectSocket();
}

DirectConnection::~DirectConnection()
{
	delete j_ptr->socket;
	delete j_ptr;
}

bool DirectConnection::open()
{
	J_D(DirectConnection);
	if( j->socket_state != QAbstractSocket::UnconnectedState )
	{
		if( j->socket_state == QAbstractSocket::ListeningState )
		{
			j->socket_state = QAbstractSocket::ConnectedState;
			QIODevice::open( ReadWrite );
			emit stateChanged( static_cast<SocketState>( j->socket_state ) );
		}
		return true;
	}
	if( j->do_lookup )
	{
		j->doLookup();
	}
	else
		j->socket->connectToHost( j->host_name, j->port );
	return true;
}

void DirectConnection::close()
{
}

Connection::SocketState DirectConnection::socketState() const
{
	J_D(const DirectConnection);
	return static_cast<SocketState>( j->socket_state );
}

Connection::SocketError DirectConnection::socketError() const
{
	J_D(const DirectConnection);
	return static_cast<SocketError>( j->socket_error );
}

qint64 DirectConnection::readData( char *data, qint64 maxlen )
{
	J_D(DirectConnection);
	return j->socket->read( data, maxlen );
}

qint64 DirectConnection::writeData( const char *data, qint64 len )
{
	J_D(DirectConnection);
	return j->socket->write( data, len );
}

J_END_NAMESPACE
