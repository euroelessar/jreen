/****************************************************************************
 *  messagefactory.cpp
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

#include "messagefactory_p.h"
#include "message_p.h"
#include "message.h"

namespace jreen {

MessageFactory::MessageFactory(Client *client) :
	StanzaFactory(client),
	m_depth(0)
{
	clear();
}

void MessageFactory::clear()
{
	m_body.clear();
	m_subject.clear();
	m_thread.clear();
	m_subtype = Message::Normal;
}

int MessageFactory::stanzaType()
{
	return StanzaPrivate::StanzaMessage;
}

Stanza::Ptr MessageFactory::createStanza()
{
	MessagePrivate *p = new MessagePrivate;
	p->from = m_from;
	p->to = m_to;
	p->id = m_id;
	p->body = m_body;
	p->subject = m_subject;
	p->subtype = m_subtype;
	p->thread = m_thread.toString();
	return Stanza::Ptr(new Message(*p));
}

void MessageFactory::serialize(Stanza *stanza, QXmlStreamWriter *writer)
{
	Message *message = static_cast<Message*>(stanza);
	if (message->subtype() == Message::Invalid)
		return;

	QString subtype;
	switch (message->subtype()) {
	case Message::Chat:
		subtype = QLatin1String("chat");
		break;
	case Message::Error:
		subtype = QLatin1String("error");
		break;
	case Message::Groupchat:
		subtype = QLatin1String("groupchat");
		break;
	case Message::Headline:
		subtype = QLatin1String("headline");
		break;
	default:
		break;
	}

	writer->writeStartElement(QLatin1String("message"));
	writeAttributes(stanza, writer);
	writer->writeAttribute(QLatin1String("type"),subtype);
	writeStanzaExtensions(stanza, writer);
	//FIXME write langmap correctly
	writer->writeTextElement(QLatin1String("subject"),message->subject());
	writer->writeTextElement(QLatin1String("body"),message->body());
	writer->writeTextElement(QLatin1String("thread"),message->thread());
	writer->writeEndElement();
}

bool MessageFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	return name == QLatin1String("message");
}

void MessageFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
										const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	m_depth++;
	if (m_depth == 1) {
		clear();
		parseAttributes(attributes);
		QStringRef subtype = attributes.value(QLatin1String("type"));
		if(subtype == QLatin1String("chat"))
			m_subtype = Message::Chat;
		else if(subtype == QLatin1String("groupchat"))
			m_subtype = Message::Groupchat;
		else if(subtype == QLatin1String("headline"))
			m_subtype = Message::Headline;
			else if(subtype == QLatin1String("error"))
			m_subtype = Message::Error;

	} else if(m_depth == 2) {
		if(name == QLatin1String("body"))
			m_state = AtBody;
		else if(name == QLatin1String("subject")) {
			m_state = AtSubject;
		}
		else if(name == QLatin1String("thread")) {
			m_state = AtThread;
		}
	}
}

void MessageFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth--;
}

void MessageFactory::handleCharacterData(const QStringRef &name)
{
	if(m_depth == 2) {
		if(m_state == AtBody)
			m_body = name.toString();
		else if(m_state == AtSubject)
			m_subject = name.toString();
		else if(m_state == AtThread)
			m_thread = name;
	}
}

} // namespace jreen
