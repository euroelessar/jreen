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

#include "iq_p.h"
#include "stanza_p.h"
#include <QStringList>

namespace Jreen
{
ConnectionIQ::ConnectionIQ(Type type, const JID& to, const QString& id)
    : IQ(type, to, id)
{
	d_func()->connection = true;
}

IQ::IQ(Type type, const JID& to, const QString& id) : Stanza(*new IQPrivate)
{
	Q_D(IQ);
	d->subtype = type;
	d->to = to;
	d->id = id;
}

IQ::IQ() : Stanza(*new IQPrivate)
{
	Q_D(IQ);
	d->subtype = IQ::Invalid;
}

IQ::IQ(IQPrivate &p) : Stanza(p)
{
}

IQ::Type IQ::subtype() const
{
	Q_D(const IQ);
	return d->subtype;
}

void IQ::accept() const
{
	Q_D(const IQ);
	d->accepted = true;
}

bool IQ::accepted() const
{
	Q_D(const IQ);
	return d->accepted;
}

}
