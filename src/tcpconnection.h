/****************************************************************************
 *  tcpconnection.h
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

#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "directconnection.h"

namespace Jreen
{

class JREEN_EXPORT TcpConnection : public DirectConnection
{
	Q_OBJECT
public:
	TcpConnection(const QString &host_name, qint16 port = -1);
	TcpConnection(const QHostAddress &address, qint16 port);
};

}

#endif // TCPCONNECTION_H
