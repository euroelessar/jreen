/****************************************************************************
 *  directconnection.h
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

#ifndef DIRECTCONNECTION_H
#define DIRECTCONNECTION_H

#include "connection.h"

class QAbstractSocket;
class QHostAddress;

J_BEGIN_NAMESPACE

class DirectConnectionPrivate;

class JREEN_EXPORT DirectConnection : public Connection
{
	Q_OBJECT
	J_DECLARE_PRIVATE(DirectConnection)

public:
	~DirectConnection();
	bool open();
	void close();
	SocketState socketState() const;
	SocketError socketError() const;

protected:
	DirectConnection( QAbstractSocket *socket, const QString &host_name, quint16 port = -1 );
	DirectConnection( QAbstractSocket *socket, const QHostAddress &address, quint16 port );
	qint64 readData( char *data, qint64 maxlen );
	qint64 writeData( const char *data, qint64 len );
	DirectConnectionPrivate *j_ptr;
};

//typedef DirectConnection<QTcpSocket> TcpConnection;
//typedef DirectConnection<QUdpSocket> UdpConnection;

J_END_NAMESPACE

#endif // DIRECTCONNECTION_H
