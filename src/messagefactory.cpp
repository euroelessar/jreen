/****************************************************************************
**
** Jreen
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#include "messagefactory_p.h"
#include "message_p.h"
#include "message.h"
#include "jstrings.h"

namespace Jreen {

static const char *message_types[] = {
		"chat",
		"error",
		"groupchat",
		"headline"
};

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
	m_state = AtMessage;
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

	QString subtype = enumToStr(message->subtype(),message_types);

	writer->writeStartElement(QLatin1String("message"));
	writeAttributes(stanza, writer);
	writer->writeAttribute(QLatin1String("type"),subtype);
	writeLangMap(QLatin1String("subject"),message->subject(),writer);
	writeLangMap(QLatin1String("body"),message->body(),writer);
	if(!message->thread().isEmpty())
		writer->writeTextElement(QLatin1String("thread"),message->thread());
	writePayloads(stanza, writer);
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
		m_subtype = strToEnum<Message::Type>(subtype,message_types);
	} else if(m_depth == 2) {
		if(name == QLatin1String("body"))
			m_state = AtBody;
		else if(name == QLatin1String("subject"))
			m_state = AtSubject;
		else if(name == QLatin1String("thread"))
			m_state = AtThread;
	}
}

void MessageFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	if (m_depth == 2)
		m_state = AtMessage;
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

} // namespace Jreen
