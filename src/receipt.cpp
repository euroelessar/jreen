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

#include "receipt.h"
#include "jstrings.h"
#include <QStringList>

namespace Jreen
{

class ReceiptPrivate
{
public:
	Receipt::Type type;
	QString id;
};

Receipt::Receipt(Type type,const QString &id) : d_ptr(new ReceiptPrivate)
{
	Q_D(Receipt);
	d->type = type;
	d->id = id;
}

Receipt::~Receipt()
{
}

Receipt::Type Receipt::type() const
{
	return d_func()->type;
}

QString Receipt::id() const
{
	return d_func()->id;
}

}
