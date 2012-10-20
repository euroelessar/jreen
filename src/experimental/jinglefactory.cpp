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

#include "jinglefactory_p.h"
#include "../jstrings.h"
#include "../client_p.h"
#include "../logger.h"

#define NS_JINGLE QLatin1String("urn:xmpp:jingle:1")
#define NS_JINGLE_RTP QLatin1String("urn:xmpp:jingle:apps:rtp:1")

namespace Jreen
{

static const char *actions[] = {
    "content-accept",
    "content-add",
    "content-modify",
    "content-reject",
    "content-remove",
    "description-info",
    "security-info",
    "session-accept",
    "session-info",
    "session-initiate",
    "session-terminate",
    "transport-accept",
    "transport-info",
    "transport-reject",
    "transport-replace"
};

static const char *senders[] = {
    "none",
    "initiator",
    "responder",
    "both"
};

JingleFactory::JingleFactory(Client *client)
    : m_client(ClientPrivate::get(client)), m_state(AtRoot), m_depth(0), m_factory(0)
{
}

bool JingleFactory::checkSupport(const QSet<QString> &features)
{
	return features.contains(NS_JINGLE) && features.contains(NS_JINGLE_RTP);
}

QStringList JingleFactory::features() const
{
	return QStringList(NS_JINGLE) << NS_JINGLE_RTP;
}

bool JingleFactory::canParse(const QStringRef &name, const QStringRef &uri,
                             const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("jingle") && uri == NS_JINGLE;
}

void JingleFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
                                       const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	++m_depth;
	if (m_depth == 1) {
		m_state = AtRoot;
		m_jingle = Jingle::Ptr::create();
		m_jingle->initiator = attributes.value(QLatin1String("initiator")).toString();
		m_jingle->sid = attributes.value(QLatin1String("sid")).toString();
		m_jingle->action = strToEnum<Jingle::Action>(attributes.value(QLatin1String("action")), actions);
	} else if (m_depth == 2 && m_state == AtRoot && name == QLatin1String("content")) {
		m_state = AtContent;
		m_content.reset(new Jingle::Content);
		m_content->name = attributes.value(QLatin1String("name")).toString();
		m_content->senders = strToEnum<Jingle::SendersType>(attributes.value(QLatin1String("senders")), senders);
		m_content->creator = strToEnum<Jingle::CreatorType>(attributes.value(QLatin1String("creator")), senders);
		if (m_content->senders == Jingle::Invalid)
			m_content->senders = Jingle::Both;
	} else if (m_depth == 3 && m_state == AtContent) {
		foreach (m_factory, m_client->factoriesByUri.values(uri.toString())) {
			Logger::debug() << uri << Payload::payloadName(m_factory->payloadType())
			         << m_factory->canParse(name, uri, attributes);
			if (m_factory->canParse(name, uri, attributes))
				break;
			else
				m_factory = 0;
		}
		if (m_factory && name == QLatin1String("transport"))
			m_state = AtTransport;
		else if (m_factory && name == QLatin1String("description"))
			m_state = AtDescription;
		else
			m_factory = 0;
		Logger::debug() << name << uri << m_factory;
	}
	if (m_factory)
		m_factory->handleStartElement(name, uri, attributes);
}

void JingleFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	if (m_factory) {
		m_factory->handleEndElement(name, uri);
		if (m_depth == 3) {
			if (m_state == AtTransport)
				m_content->transports << m_factory->createPayload();
			else if (m_state == AtDescription)
				m_content->description = m_factory->createPayload();
			else
				Q_ASSERT(!"Unknown state with existen factory");
			m_factory = 0;
			m_state = AtContent;
		}
	} else if (m_depth == 2 && m_state == AtContent) {
		m_jingle->contents << *m_content;
		m_content.reset();
		m_state = AtRoot;
	}
	
	--m_depth;
}

void JingleFactory::handleCharacterData(const QStringRef &text)
{
	if (m_factory)
		m_factory->handleCharacterData(text);
}

void JingleFactory::serialize(Payload *obj, QXmlStreamWriter *writer)
{
	Jingle *jingle = se_cast<Jingle*>(obj);
	if (!jingle) return;
	writer->writeStartElement(QLatin1String("jingle"));
	writer->writeDefaultNamespace(NS_JINGLE);
	writer->writeAttribute(QLatin1String("action"), enumToStr(jingle->action, actions));
	if (jingle->initiator.isValid())
		writer->writeAttribute(QLatin1String("initiator"), jingle->initiator);
	if (jingle->responder.isValid())
		writer->writeAttribute(QLatin1String("responder"), jingle->responder);
	writer->writeAttribute(QLatin1String("sid"), jingle->sid);
	for (int i = 0; i < jingle->contents.size(); ++i) {
		const Jingle::Content &content = jingle->contents.at(i);
		writer->writeStartElement(QLatin1String("content"));
		writer->writeAttribute(QLatin1String("creator"), enumToStr(content.creator, senders));
		writer->writeAttribute(QLatin1String("senders"), enumToStr(content.senders, senders));
		writer->writeAttribute(QLatin1String("name"), content.name);
		AbstractPayloadFactory *factory = 0;
		if (content.description) {
			factory = m_client->factories.value(content.description->payloadType());
			if (factory)
				factory->serialize(content.description.data(), writer);
		} else {
			Logger::debug() << "No description";
		}
		for (int j = 0; j < content.transports.size(); ++j) {
			Payload *payload = content.transports.at(j).data();
			factory = m_client->factories.value(payload->payloadType());
			if (factory)
				factory->serialize(payload, writer);
		}
		writer->writeEndElement();
	}
	writer->writeEndElement();
}

Payload::Ptr JingleFactory::createPayload()
{
	Payload::Ptr result = m_jingle;
	m_jingle.clear();
	return result;
}

}
