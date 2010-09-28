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

namespace jreen
{

J_STRING(nick)

Nickname::Nickname(const QDomElement &node)
{
	if(node.isNull())
		return;
	m_nick = node.text();
}

Nickname::Nickname(const QString &nick) : m_nick(nick)
{
}

QDomElement Nickname::node(QDomDocument *doc) const
{
	if(m_nick.isEmpty())
		return QDomElement();
	QDomElement node = createElement(doc, nick_str, m_nick);
	node.setAttribute(ConstString::xmlns, ConstString::xmlns_nickname);
	return node;
}


}
