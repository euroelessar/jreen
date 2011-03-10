/****************************************************************************
 *  chatstatefactory.cpp
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

#include "chatstatefactory_p.h"
#include <QMap>
#include <QXmlStreamReader>
#include <QStringList>
#include "jstrings.h"

#define NS_CHATSTATE QLatin1String("http://jabber.org/protocol/chatstates")

namespace Jreen {

const char *state_strings[] = {"active",
							   "inactive",
							   "gone",
							   "composing",
							   "paused"};

ChatStateFactory::ChatStateFactory()
{
	m_state = ChatState::InActive;
}

ChatStateFactory::~ChatStateFactory()
{

}

QStringList ChatStateFactory::features() const
{
	return QStringList(NS_CHATSTATE);
}

bool ChatStateFactory::canParse(const QStringRef &name,
								const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return (strToEnum(name.toString(),state_strings) != -1) && (uri == NS_CHATSTATE);
}

void ChatStateFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	m_state = strToEnum<ChatState::State>(name.toString(),state_strings);
}

void ChatStateFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
}

void ChatStateFactory::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

void ChatStateFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	ChatState *state = se_cast<ChatState*>(extension);
	writer->writeStartElement(enumToStr(state->state(),state_strings));
	writer->writeDefaultNamespace(NS_CHATSTATE);
	writer->writeEndElement();
}

StanzaExtension::Ptr ChatStateFactory::createExtension()
{
	return StanzaExtension::Ptr(new ChatState(m_state));
}

} // namespace Jreen
