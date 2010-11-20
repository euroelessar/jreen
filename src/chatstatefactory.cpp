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

#define NS_CHATSTATE QLatin1String("http://jabber.org/protocol/chatstates")

namespace jreen {

QMap<int,QString> stateStrings()
{
	QMap<int,QString> states;
	states.insert(ChatState::Active,QLatin1String("active"));
	states.insert(ChatState::InActive,QLatin1String("inactive"));
	states.insert(ChatState::Composing,QLatin1String("composing"));
	states.insert(ChatState::Paused,QLatin1String("paused"));
	states.insert(ChatState::Gone,QLatin1String("gone"));
	return states;
}

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
	return (stateStrings().key(name.toString(),-1) != -1) && (uri == NS_CHATSTATE);
}

void ChatStateFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	m_state = static_cast<ChatState::State>(stateStrings().key(name.toString()));
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
	writer->writeStartElement(stateStrings().value(state->state()));
	writer->writeDefaultNamespace(NS_CHATSTATE);
	writer->writeEndElement();
}

StanzaExtension::Ptr ChatStateFactory::createExtension()
{
	return StanzaExtension::Ptr(new ChatState(m_state));
}

} // namespace jreen
