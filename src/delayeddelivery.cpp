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

#include "delayeddelivery.h"
#include "util.h"
#include "jstrings.h"
#include <QXmlStreamWriter>

namespace Jreen
{

class DelayedDeliveryPrivate
{
public:
	JID from;
	QDateTime dateTime;
	QString reason;
};

DelayedDelivery::DelayedDelivery(const JID &from, const QDateTime &dateTime, const QString &reason)
    : d_ptr(new DelayedDeliveryPrivate)
{
	Q_D(DelayedDelivery);
	d->from = from;
	d->dateTime = dateTime;
	d->reason = reason;
}

DelayedDelivery::~DelayedDelivery()
{
}

JID DelayedDelivery::from() const
{
	return d_func()->from;
}

QString DelayedDelivery::reason() const
{
	return d_func()->reason;
}

QDateTime DelayedDelivery::dateTime() const
{
	return d_func()->dateTime;
}

}
