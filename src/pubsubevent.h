/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** Jreen is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Jreen is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Jreen.  If not, see <http://www.gnu.org/licenses/>.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef JREEN_PUBSUBEVENT_H
#define JREEN_PUBSUBEVENT_H

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

#endif // JREEN_PUBSUBEVENT_H
