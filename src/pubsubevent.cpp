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

#include "pubsubevent.h"
#include "pubsubmanager_p.h"

namespace Jreen
{
namespace PubSub
{
class EventPrivate
{
public:
	QList<Payload::Ptr> items;
	QString node;
};

Event::Event(const QString &node) : d_ptr(new EventPrivate)
{
	Q_D(Event);
	d->node = node;
}

Event::Event(const Payload::Ptr &item) : d_ptr(new EventPrivate)
{
	Q_D(Event);
	d->items << item;
}

Event::Event(const QList<Payload::Ptr> &items) : d_ptr(new EventPrivate)
{
	Q_D(Event);
	d->items = items;
}

Event::~Event()
{
}

void Event::setNode(const QString &node)
{
	d_func()->node = node;
}

QString Event::node() const
{
	return d_func()->node;
}

void Event::addItem(Payload *item)
{
	d_func()->items << Payload::Ptr(item);
}

void Event::addItem(const Payload::Ptr &item)
{
	d_func()->items << item;
}

QList<Payload::Ptr> Event::items() const
{
	return d_func()->items;
}
}
}
