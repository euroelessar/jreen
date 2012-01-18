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

#ifndef DELAYEDDELIVERY_H
#define DELAYEDDELIVERY_H

#include "stanzaextension.h"
#include "jid.h"
#include <QDateTime>

namespace Jreen
{

class DelayedDeliveryPrivate;

//XEP 0203
//http://xmpp.org/extensions/xep-0203.html

class JREEN_EXPORT DelayedDelivery : public Payload
{
	Q_DECLARE_PRIVATE(DelayedDelivery)
	J_PAYLOAD(Jreen::DelayedDelivery)
public:
	DelayedDelivery(const JID &from, const QDateTime &dateTime, const QString &reason = QString());
	virtual ~DelayedDelivery();

	JID from() const;
	QString reason() const;
	QDateTime dateTime() const;
private:
	QScopedPointer<DelayedDeliveryPrivate> d_ptr;
};

}

#endif // DELAYEDDELIVERY_H
