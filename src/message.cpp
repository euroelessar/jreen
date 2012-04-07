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

#include "message.h"
#include "message_p.h"
#include "delayeddelivery.h"
#include "util.h"
#include <QStringList>

namespace Jreen
{

Message::Message(Message::Type type)
    : Stanza(*new MessagePrivate)
{
	Q_D(Message);
	d->subtype = type;
}

Message::Message(Type type, const JID& to, const QString &body, const QString &subject, const QString &thread, const QString &xmllang)
	: Stanza(*new MessagePrivate)
{
	Q_D(Message);
	d->subtype = type;
	d->to = to;
	d->thread = thread;
	d->body[xmllang] = body;
	d->subject[xmllang] = subject;
}

Message::Message(MessagePrivate &p) : Stanza(p)
{
}

Message::Type Message::subtype() const
{
	Q_D(const Message);
	return d->subtype;
}

QString Message::body(const QString &lang) const
{
	Q_D(const Message);
	return d->body.value(lang);
}

void Message::setBody(const QString &text, const QString &lang)
{
	d_func()->body.insert(lang, text);
}

QString Message::subject(const QString &lang) const
{
	Q_D(const Message);
	return d->subject.value(lang);
}

void Message::setSubject(const QString &text, const QString &lang)
{
	d_func()->subject.insert(lang, text);
}

QString Message::thread() const
{
	Q_D(const Message);
	return d->thread;
}

void Message::setThread(const QString &thread)
{
	Q_D(Message);
	d->thread = thread;
}

void Message::setID(const QString &id)
{
	Q_D(Message);
	d->id = id;
}

DelayedDelivery::Ptr Message::when() const
{
	return payload<DelayedDelivery>();
}

}
