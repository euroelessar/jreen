/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "forwardedfactory_p.h"

#define NS_FORWARDED QLatin1String("urn:xmpp:forward:0")

namespace Jreen
{

ForwardedFactory::ForwardedFactory(Client *client)
    : m_messageFactory(client)
{
	m_depth = 0;
	m_state = AtUnknown;
}

QStringList ForwardedFactory::features() const
{
	return QStringList(NS_FORWARDED);
}

bool ForwardedFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("forwarded") && uri == NS_FORWARDED;
}

void ForwardedFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	++m_depth;
	if (m_depth == 1) {
		m_forwarded.reset(new Forwarded);
	} else if (m_depth == 2) {
		if (m_delayedFactory.canParse(name, uri, attributes))
			m_state = AtDelayed;
		else if (m_messageFactory.canParse(name, uri, attributes))
			m_state = AtMessage;
		else
			m_state = AtUnknown;
	}
	if (m_state == AtDelayed)
		m_delayedFactory.handleStartElement(name, uri, attributes);
	else if (m_state == AtMessage)
		m_messageFactory.handleStartElement(name, uri, attributes);
}

void ForwardedFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	if (m_state == AtDelayed)
		m_delayedFactory.handleEndElement(name, uri);
	else if (m_state == AtMessage)
		m_messageFactory.handleEndElement(name, uri);

	if (m_depth == 2) {
		if (m_state == AtDelayed) {
			m_forwarded->setTime(m_delayedFactory.createPayload().staticCast<DelayedDelivery>());
		} else if (m_state == AtMessage) {
			Stanza::Ptr message = m_messageFactory.createStanza();
			m_forwarded->setMessage(*message.staticCast<Message>().data());
		}
		m_state = AtUnknown;
	}
	--m_depth;
}

void ForwardedFactory::handleCharacterData(const QStringRef &text)
{
	if (m_state == AtDelayed)
		m_delayedFactory.handleCharacterData(text);
	else if (m_state == AtMessage)
		m_messageFactory.handleCharacterData(text);
}

void ForwardedFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	Forwarded *forwarded = payload_cast<Forwarded*>(extension);
	writer->writeStartElement(QLatin1String("forwarded"));
	writer->writeDefaultNamespace(NS_FORWARDED);
	if (forwarded->time())
		m_delayedFactory.serialize(forwarded->time().data(), writer);
	Message message = forwarded->message();
	m_messageFactory.serialize(&message, writer);
	writer->writeEndElement();
}

Payload::Ptr ForwardedFactory::createPayload()
{
	return Payload::Ptr(m_forwarded.take());
}

} // namespace Jreen
