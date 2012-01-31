/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include <QStringList>
#include <QXmlStreamWriter>
#include "capabilities.h"
#include "capabilitiesfactory_p.h"

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

QString Capabilities::calculatedNode() const
{
	//return CapabilitesFactory::hashValue(this);
	return QString();
}

}
