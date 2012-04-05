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

#include "stanza.h"
#include "stanza_p.h"
#include "error.h"
#include <QTextStream>

namespace Jreen
{

Stanza::Stanza(const Stanza &stanza)
{
	if(stanza.d_ptr)
		stanza.d_ptr->ref.ref();
	d_ptr = stanza.d_ptr;
}

Stanza &Stanza::operator =(const Stanza &stanza)
{
	if(stanza.d_ptr)
		stanza.d_ptr->ref.ref();
	if(!d_ptr->ref.deref())
		delete d_ptr;
	d_ptr = stanza.d_ptr;
	return *this;
}

Stanza::Stanza(StanzaPrivate &sp)
{
	d_ptr = &sp;
}

Stanza::~Stanza()
{
	if(d_ptr && !d_ptr->ref.deref())
		delete d_ptr;
}

void Stanza::setFrom(const JID &jid)
{
	d_ptr->from = jid;
}

const JID &Stanza::from() const
{
	return d_ptr->from;
}

const JID &Stanza::to() const
{
	return d_ptr->to;
}

const QString &Stanza::id() const
{
	return d_ptr->id;
}

void Stanza::addExtension(Payload::Ptr se)
{
	d_ptr->extensions.insert(se->payloadType(), se);
}

PayloadList Stanza::payloads() const
{
	return d_ptr->extensions;
}

void Stanza::removePayloads()
{
	d_ptr->extensions.clear();
}

void Stanza::removePayload(int id)
{
	d_ptr->extensions.remove(id);
}

Error::Ptr Stanza::error() const
{
	return payload<Error>();
}

}
