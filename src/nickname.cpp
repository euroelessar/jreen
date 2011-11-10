/****************************************************************************
 *  nickname.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "nickname.h"
#include "jstrings.h"

namespace Jreen
{

class NicknamePrivate
{
public:
	QString nick;
};

Nickname::Nickname(const QString &nick) : d_ptr(new NicknamePrivate)
{
	d_func()->nick = nick;
}

Nickname::~Nickname()
{
}

QString Nickname::nick() const
{
	return d_func()->nick;
}

}
