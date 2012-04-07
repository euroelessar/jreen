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

#include "messagefactory_p.h"
#include "message_p.h"
#include "message.h"
#include "jstrings.h"

namespace Jreen {

static const char *message_types[] = {
    "chat",
    "error",
    "groupchat",
    "headline",
    "normal"
};

MessageFactory::MessageFactory(Client *client) :
    StanzaFactory(client), m_state(AtMessage)
{
}

int MessageFactory::stanzaType()
{
	return StanzaPrivate::StanzaMessage;
}

Stanza::Ptr MessageFactory::createStanza()
{
	return Stanza::Ptr(new Message(*static_cast<MessagePrivate*>(m_stanza.take())));
}

void MessageFactory::serialize(Stanza *stanza, QXmlStreamWriter *writer)
{
	if (!StanzaPrivate::get(*stanza)->tokens.isEmpty()) {
		StanzaFactory::serialize(stanza, writer);
		return;
	}
	Message *message = static_cast<Message*>(stanza);
	if (message->subtype() == Message::Invalid)
		return;

	QLatin1String subtype = enumToStr(message->subtype(),message_types);

	writer->writeStartElement(QLatin1String("message"));
	writeAttributes(stanza, writer);
	if (subtype != QLatin1String(""))
		writer->writeAttribute(QLatin1String("type"), subtype);
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
	m_depth++;
	if (m_depth == 1)
		m_stanza.reset(new MessagePrivate);
	StanzaFactory::handleStartElement(name, uri, attributes);
	if (m_depth == 1) {
		m_state = AtMessage;
		MessagePrivate *p = static_cast<MessagePrivate*>(m_stanza.data());
		QStringRef subtype = attributes.value(QLatin1String("type"));
		if (subtype.isEmpty())
			p->subtype = Message::Normal;
		else
			p->subtype = strToEnum<Message::Type>(subtype, message_types);
		if (p->subtype < 0)
			p->subtype = Message::Invalid;
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
	StanzaFactory::handleEndElement(name, uri);
	if (m_depth == 2)
		m_state = AtMessage;
	m_depth--;
}

void MessageFactory::handleCharacterData(const QStringRef &name)
{
	StanzaFactory::handleCharacterData(name);
	if(m_depth == 2) {
		MessagePrivate *p = static_cast<MessagePrivate*>(m_stanza.data());
		if(m_state == AtBody)
			p->body = name.toString();
		else if(m_state == AtSubject)
			p->subject = name.toString();
		else if(m_state == AtThread)
			p->thread = name.toString();
	}
}

} // namespace Jreen
