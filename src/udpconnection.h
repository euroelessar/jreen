/****************************************************************************
 *  udpconnection.h
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

#ifndef UDPCONNECTION_H
#define UDPCONNECTION_H

#include "directconnection.h"

namespace Jreen
{

class JREEN_EXPORT UdpConnection : public DirectConnection
{
	Q_OBJECT
public:
	UdpConnection(const QString &host_name, qint16 port = -1);
	UdpConnection(const QHostAddress &address, qint16 port);
};

}

#endif // UDPCONNECTION_H
