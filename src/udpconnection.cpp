/****************************************************************************
 *  udpconnection.cpp
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

#include "udpconnection.h"
#include <QUdpSocket>

namespace Jreen
{

UdpConnection::UdpConnection(const QString &host_name, qint16 port)
		: DirectConnection(new QUdpSocket, host_name, port)
{
}

UdpConnection::UdpConnection(const QHostAddress &address, qint16 port)
		: DirectConnection(new QUdpSocket, address, port)
{
}

}
