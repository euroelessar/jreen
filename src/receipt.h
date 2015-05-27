/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef JREEN_RECEIPT_H
#define JREEN_RECEIPT_H

#include "stanzaextension.h"

namespace Jreen
{

class ReceiptPrivate;
class JREEN_EXPORT Receipt : public Payload
{
	Q_DECLARE_PRIVATE(Receipt)
	J_PAYLOAD(Jreen::Receipt)
public:
	enum Type
	{
		Request		= 0,
		Received	= 1,
		Invalid		= -1
	};

	Receipt(Type type,const QString &id = QString());
	~Receipt();

	Type type() const;
	QString id() const;
private:
	QScopedPointer<ReceiptPrivate> d_ptr;
};

}

#endif // JREEN_RECEIPT_H
