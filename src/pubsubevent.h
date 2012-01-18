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

#ifndef PUBSUBEVENT_H
#define PUBSUBEVENT_H

#include "stanzaextension.h"

namespace Jreen
{
namespace PubSub
{
class EventPrivate;
class JREEN_EXPORT Event : public Payload
{
	Q_DECLARE_PRIVATE(Event)
	J_PAYLOAD(Jreen::PubSub::Event)
	public:
		Event(const QString &node = QString());
	Event(const Payload::Ptr &item);
	Event(const QList<Payload::Ptr> &items);
	~Event();

	void setNode(const QString &node);
	QString node() const;
	void addItem(Payload *item);
	void addItem(const Payload::Ptr &item);
	QList<Payload::Ptr> items() const;
private:
	QScopedPointer<EventPrivate> d_ptr;
};
}
}

#endif // PUBSUBEVENT_H
