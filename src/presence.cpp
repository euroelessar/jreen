/****************************************************************************
 *  presence.cpp
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

#include "presence_p.h"
#include "capabilities.h"

namespace Jreen
{

Presence::Presence(Type type, const JID& to,
				   const QString &status, int priority, const QString &xmllang)
	: Stanza(*new PresencePrivate)
{
	Q_D(Presence);
	d->subtype = type;
	d->to = to;
	d->priority = priority;
	d->status[xmllang] = status;
}

Presence::Presence(Type type, const JID& to,const LangMap &status,int priority)
	: Stanza(*new PresencePrivate)
{
	Q_D(Presence);
	d->subtype = type;
	d->to = to;
	d->priority = priority;
	d->status = status;
}

Presence::Presence(PresencePrivate &p) : Stanza(p)
{
}

void Presence::setSubtype(Type subtype)
{
	d_func()->subtype = subtype;
}

Presence::Type Presence::subtype() const
{
	Q_D(const Presence);
	return d->subtype;
}

bool Presence::isSubscription() const
{
	Q_D(const Presence);
	return d->subtype == Subscribe
	        || d->subtype == Unsubscribe
	        || d->subtype == Subscribed
	        || d->subtype == Unsubscribed;
}

const Capabilities *Presence::capabilities() const
{
	return findExtension<Capabilities>().data();
}

const QString &Presence::status(const QString &lang) const
{
	Q_D(const Presence);
	return d->status.value(lang);
}

void Presence::addStatus(const QString &status, const QString &lang)
{
	Q_D(Presence);
	d->status[lang] = status;
}

void Presence::resetStatus()
{
	Q_D(Presence);
	d->status.clear();
	d->status.clear();
}

int Presence::priority() const
{
	Q_D(const Presence);
	return d->priority;
}

void Presence::setPriority(int priority)
{
	Q_D(Presence);
	d->priority = qBound(-128, priority, 127);
}

}
