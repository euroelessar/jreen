/****************************************************************************
 *  capabilities.cpp
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

#include <QStringList>
#include <QXmlStreamWriter>
#include "capabilities.h"

namespace Jreen
{

class CapabilitiesPrivate
{
public:
	QString ver;
	QString node;
};
	
Capabilities::Capabilities(const QString &ver, const QString &node) : d_ptr(new CapabilitiesPrivate)
{
	Q_D(Capabilities);
	d->ver = ver;
	d->node = node;
}

Capabilities::~Capabilities()
{
}

void Capabilities::setNode(const QString &node)
{
	d_func()->node = node;
}

QString Capabilities::node() const
{
	return d_func()->node;
}

QString Capabilities::ver() const
{
	return d_func()->ver;
}

}
