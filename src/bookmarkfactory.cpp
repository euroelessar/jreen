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

#include "bookmarkfactory_p.h"
#include <QXmlStreamWriter>
#include <QStringList>
#include "util.h"
#include "jstrings.h"
#define NS_BOOKMARKS QLatin1String("storage:bookmarks")

namespace Jreen {

static const char *autojoin_types[] = {
	"false",
	"true"
};

static const char *bookmark_strings[] = {
	"nick",
	"password"
};

using namespace Util;

BookmarkFactory::BookmarkFactory()
{
	m_depth = 0;
	m_state = AtNowhere;
}

BookmarkFactory::~BookmarkFactory()
{
}

QStringList BookmarkFactory::features() const
{
	return QStringList(NS_BOOKMARKS);
}

bool BookmarkFactory::canParse(const QStringRef &name, const QStringRef &uri,
							   const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("storage") && uri == NS_BOOKMARKS;
}

void BookmarkFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
										 const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	m_depth++;
	if(m_depth == 1) {
		m_state = AtNowhere;
		m_bookmark.reset(new Bookmark);
	} else if (m_depth == 2 && name == QLatin1String("conference")) {
		m_state = AtConference;
		m_conference = Bookmark::Conference();
		m_conference.setName(attributes.value(QLatin1String("name")).toString());
		QStringRef autojoin = attributes.value(QLatin1String("autojoin"));
		m_conference.setAutojoin(autojoin == QLatin1String("true"));
		m_conference.setJid(attributes.value(QLatin1String("jid")).toString());
	} else if(m_depth == 3 && m_state == AtConference) {
		m_state = strToEnum<State>(name, bookmark_strings);
	}
}

void BookmarkFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	if (m_state < AtNowhere && m_depth == 3)
		m_state = AtConference;
	else if (m_state == AtConference && m_depth == 2)
		m_bookmark->addConference(m_conference);
	m_depth--;
}

void BookmarkFactory::handleCharacterData(const QStringRef &text)
{
	switch(m_state) {
	case AtNick:
		m_conference.setNick(text.toString());
		break;
	case AtPassword:
		m_conference.setPassword(text.toString());
		break;
	default:
		break;
	}
}

void BookmarkFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	Bookmark *bookmark = se_cast<Bookmark*>(extension);
	writer->writeStartElement(QLatin1String("storage"));
	writer->writeDefaultNamespace(NS_BOOKMARKS);
	foreach (const Bookmark::Conference &conf, bookmark->conferences()) {
		writer->writeStartElement(QLatin1String("conference"));
		writeAttribute(writer,QLatin1String("jid"), conf.jid().full());
		writeAttribute(writer,QLatin1String("name"), conf.name());
		writeAttribute(writer,QLatin1String("autojoin"), enumToStr(conf.autojoin(), autojoin_types));
		writeTextElement(writer,QLatin1String("nick"), conf.nick());
		writeTextElement(writer,QLatin1String("password"), conf.password());
		writer->writeEndElement();
	}
	writer->writeEndElement();
}

Payload::Ptr BookmarkFactory::createPayload()
{
	return Payload::Ptr(m_bookmark.take());
}


} // namespace Jreen

