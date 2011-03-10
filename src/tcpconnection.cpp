/****************************************************************************
 *  tcpconnection.cpp
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

#include "tcpconnection.h"
#include <QTcpSocket>

namespace Jreen
{

TcpConnection::TcpConnection(const QString &host_name, qint16 port)
		: DirectConnection(new QTcpSocket, host_name, port)
{
}

TcpConnection::TcpConnection(const QHostAddress &address, qint16 port)
		: DirectConnection(new QTcpSocket, address, port)
{
}

}
