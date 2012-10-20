/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "forwarded.h"

namespace Jreen
{

class ForwardedPrivate
{
public:
	ForwardedPrivate(const Message &m, const DelayedDelivery::Ptr &t)
	    : message(m), time(t) {}
	Message message;
	DelayedDelivery::Ptr time;
};

Forwarded::Forwarded(const Message &message, const DelayedDelivery::Ptr &time)
    : d_ptr(new ForwardedPrivate(message, time))
{
}

Forwarded::~Forwarded()
{
}

Message Forwarded::message() const
{
	return d_func()->message;
}

void Forwarded::setMessage(const Message &message)
{
	d_func()->message = message;
}

DelayedDelivery::Ptr Forwarded::time() const
{
	return d_func()->time;
}

void Forwarded::setTime(const DelayedDelivery::Ptr &time)
{
	d_func()->time = time;
}
} // namespace Jreen
