/****************************************************************************
 *  bookmark.cpp
 *
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

#include "bookmark.h"

namespace jreen {

class BookmarkPrivate
{
public:
	JID jid;
	QString name;
	QString nick;
	QString password;
	bool autojoin;
};

Bookmark::Bookmark(const JID &jid, const QString &name, const QString &nick) :
	d_ptr(new BookmarkPrivate)
{
	Q_D(Bookmark);
	d->jid = jid;
	d->name = name;
	d->nick = nick;
	d->autojoin = false;
}

Bookmark::~Bookmark()
{

}

void Bookmark::setName(const QString &name)
{
	d_func()->name = name;
}

void Bookmark::setNick(const QString &nick)
{
	d_func()->nick = nick;
}

void Bookmark::setPassword(const QString &password)
{
	d_func()->password = password;
}

QString Bookmark::name() const
{
	return d_func()->name;
}

QString Bookmark::nick() const
{
	return d_func()->nick;
}

QString Bookmark::password() const
{
	return d_func()->password;
}

JID Bookmark::jid() const
{
	return d_func()->jid;
}

void Bookmark::setAutojoin(bool set)
{
	d_func()->autojoin = set;
}

bool Bookmark::autojoin() const
{
	return d_func()->autojoin;
}


} // namespace jreen
