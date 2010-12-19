/****************************************************************************
 *  bookmarkfactory.cpp
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

#include "bookmarkfactory.h"
#include <QXmlStreamWriter>
#include <QStringList>
#include "util.h"
#include "jstrings.h"
#define NS_BOOKMARKS QLatin1String("storage:bookmarks")

namespace jreen {

static const char *autojoin_types[] = {
	"true",
	"false"
};

static const char *bookmark_strings[] = {
	"nick",
	"password"
};

using namespace Util;

BookmarkFactory::BookmarkFactory()
{
	m_depth = 0;
	clear();
}

BookmarkFactory::~BookmarkFactory()
{

}

void BookmarkFactory::clear()
{
	m_jid.clear();
	m_autoJoin = false;
	m_name.clear();
	m_nick.clear();
	m_password.clear();
}

QStringList BookmarkFactory::features() const
{
	return QStringList(NS_BOOKMARKS);
}

bool BookmarkFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("storage") && uri == NS_BOOKMARKS;
}

void BookmarkFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	m_depth++;
	if(m_depth == 1) {
		m_name = attributes.value(QLatin1String("name")).toString();
		m_autoJoin = strToEnum<bool>(attributes.value(QLatin1String("autojoin")),autojoin_types);
		m_jid = attributes.value(QLatin1String("jid")).toString();
	} else if(m_depth == 2) {
		m_state = strToEnum<State>(name,bookmark_strings);
	}
}

void BookmarkFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth--;
}

void BookmarkFactory::handleCharacterData(const QStringRef &text)
{
	switch(m_state) {
	case AtNick:
		m_nick = text.toString();
	case AtPassword:
		m_password = text.toString();
	}
}

void BookmarkFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	Bookmark *bookmark = se_cast<Bookmark*>(extension);
	writer->writeStartElement(QLatin1String("storage"));
	writer->writeDefaultNamespace(NS_BOOKMARKS);
	writer->writeStartElement(QLatin1String("conference"));
	writeAttribute(writer,QLatin1String("jid"),bookmark->jid().full());
	writeAttribute(writer,QLatin1String("name"),bookmark->name());
	writeAttribute(writer,QLatin1String("autojoin"),enumToStr(bookmark->autojoin(),autojoin_types));
	writeTextElement(writer,QLatin1String("nick"),bookmark->nick());
	writeTextElement(writer,QLatin1String("password"),bookmark->password());
	writer->writeEndElement();
	writer->writeEndElement();
}

StanzaExtension::Ptr BookmarkFactory::createExtension()
{
	Bookmark *bookmark = new Bookmark(m_jid,m_name,m_nick);
	bookmark->setAutojoin(m_autoJoin);
	bookmark->setPassword(m_password);
	clear();
	return StanzaExtension::Ptr(bookmark);
}


} // namespace jreen

