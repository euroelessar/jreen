/****************************************************************************
**
** Jreen
**
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

#include "bookmark.h"

namespace Jreen {
class Bookmark::ConferencePrivate : public QSharedData
{
public:
	ConferencePrivate() : autojoin(false) {}
	ConferencePrivate(const ConferencePrivate &o)
		: QSharedData(o), jid(o.jid), name(o.name), nick(o.nick),
		  password(o.password), autojoin(o.autojoin) {}
	JID jid;
	QString name;
	QString nick;
	QString password;
	bool autojoin;
};

Bookmark::Conference::Conference() : d_ptr(new Bookmark::ConferencePrivate)
{
}

Bookmark::Conference::Conference(const QString &name, const JID &jid, const QString &nick,
								 const QString &password, bool autojoin)
	: d_ptr(new Bookmark::ConferencePrivate)
{
	d_ptr->name = name;
	setJid(jid);
	d_ptr->nick = nick;
	d_ptr->password = password;
	d_ptr->autojoin = autojoin;
}

Bookmark::Conference::Conference(const Bookmark::Conference &o) : d_ptr(o.d_ptr)
{
}

Bookmark::Conference &Bookmark::Conference::operator =(const Bookmark::Conference &o)
{
	d_ptr = o.d_ptr;
	return *this;
}

Bookmark::Conference::~Conference()
{
}

bool Bookmark::Conference::operator ==(const Conference &o)
{
	return d_ptr == o.d_ptr;
}

bool Bookmark::Conference::operator !=(const Conference &o)
{
	return d_ptr != o.d_ptr;
}

bool Bookmark::Conference::isValid() const
{
	return d_ptr->jid.isValid() && !d_ptr->nick.isEmpty();
}

void Bookmark::Conference::setJid(const JID &jid)
{
	if (!jid.isBare())
		d_ptr->jid = jid.bareJID();
	else
		d_ptr->jid = jid;
}

void Bookmark::Conference::setName(const QString &name)
{
	d_ptr->name = name;
}

void Bookmark::Conference::setNick(const QString &nick)
{
	d_ptr->nick = nick;
}

void Bookmark::Conference::setPassword(const QString &password)
{
	d_ptr->password = password;
}

QString Bookmark::Conference::name() const
{
	return d_ptr->name;
}

QString Bookmark::Conference::nick() const
{
	return d_ptr->nick;
}

QString Bookmark::Conference::password() const
{
	return d_ptr->password;
}

JID Bookmark::Conference::jid() const
{
	return d_ptr->jid;
}

void Bookmark::Conference::setAutojoin(bool set)
{
	d_ptr->autojoin = set;
}

bool Bookmark::Conference::autojoin() const
{
	return d_ptr->autojoin;
}

class BookmarkPrivate
{
public:
	QList<Bookmark::Conference> conferences;
};

Bookmark::Bookmark() : d_ptr(new BookmarkPrivate)
{
}

Bookmark::~Bookmark()
{

}

QList<Bookmark::Conference> Bookmark::conferences() const
{
	return d_func()->conferences;
}

void Bookmark::addConference(const Conference &conf)
{
	d_func()->conferences << conf;
}

void Bookmark::setConferences(const QList<Conference> &conferences)
{
	d_func()->conferences = conferences;
}
} // namespace Jreen
