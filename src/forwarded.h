/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef JREEN_FORWARDED_H
#define JREEN_FORWARDED_H

#include "message.h"

namespace Jreen
{

class ForwardedPrivate;

class JREEN_EXPORT Forwarded : public Payload
{
	J_PAYLOAD(Jreen::Forwarded)
	Q_DECLARE_PRIVATE(Forwarded)
public:
	Forwarded(const Message &message = Message(), const DelayedDelivery::Ptr &time = DelayedDelivery::Ptr());
	~Forwarded();

	Message message() const;
	void setMessage(const Message &message);

	DelayedDelivery::Ptr time() const;
	void setTime(const DelayedDelivery::Ptr &time);

private:
	QScopedPointer<ForwardedPrivate> d_ptr;
};

} // namespace Jreen

#endif // JREEN_FORWARDED_H
