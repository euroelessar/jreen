/****************************************************************************
 *  message.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "message.h"
#include "message_p.h"
#include "delayeddelivery.h"
#include "util.h"
#include <QStringList>

namespace Jreen
{

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

const QString &Message::body(const QString &lang) const
{
	Q_D(const Message);
	return d->body.value(lang);
}

void Message::setBody(const QString &text, const QString &lang)
{
	d_func()->body.insert(lang, text);
}

const QString &Message::subject(const QString &lang) const
{
	Q_D(const Message);
	return d->subject.value(lang);
}

void Message::setSubject(const QString &text, const QString &lang)
{
	d_func()->subject.insert(lang, text);
}

const QString &Message::thread() const
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

const DelayedDelivery *Message::when() const
{
	return findExtension<DelayedDelivery>().data();
}

}
