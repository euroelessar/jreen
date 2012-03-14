/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "pubsubeventfactory_p.h"
#include <QXmlStreamWriter>
#include <QStringList>
#include "logger.h"

#define NS_EVENT QLatin1String("http://jabber.org/protocol/pubsub#event")

namespace Jreen
{
namespace PubSub
{
EventFactory::EventFactory(QList<AbstractPayloadFactory*> &factories) : m_factories(factories)
{
	m_depth = 0;
	m_state = AtNowhere;
}

EventFactory::~EventFactory()
{
}

QStringList EventFactory::features() const
{
	return QStringList(NS_EVENT);
}

bool EventFactory::canParse(const QStringRef &name, const QStringRef &uri,
							const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("event") && uri == NS_EVENT;
}

void EventFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
									  const QXmlStreamAttributes &attributes)
{
	m_depth++;
	if (m_depth == 1) {
		m_event.reset(new Event);
	} if (m_depth == 2 && name == QLatin1String("items")) {
		m_factory = findFactory(attributes.value(QLatin1String("node")));
		m_state = m_factory ? AtItems : AtNowhere;
	} else if (m_depth == 3 && m_state == AtItems && name == QLatin1String("item")) {
		m_state = AtItem;
	} else if (m_depth == 4 && m_state == AtItem && m_factory->canParse(name, uri, attributes)) {
		m_state = AtEntity;
	}
	if (m_state == AtEntity)
		m_factory->handleStartElement(name, uri, attributes);
}

void EventFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	if (m_state == AtEntity)
		m_factory->handleEndElement(name, uri);
	if (m_depth == 2 && m_state == AtItems) {
		m_state = AtNowhere;
	} else if (m_depth == 3 && m_state == AtItem) {
		m_state = AtItems;
	} else if (m_depth == 4 && m_state == AtEntity) {
		m_state = AtItem;
		m_event->addItem(m_factory->createPayload());
	}
	m_depth--;
}

void EventFactory::handleCharacterData(const QStringRef &text)
{
	if (m_state == AtEntity)
		m_factory->handleCharacterData(text);
}

void EventFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	Event *event = se_cast<Event*>(extension);
	AbstractPayloadFactory *factory = 0;
	QString node;
	const QList<Payload::Ptr> items = event->items();
	if (event->node().isEmpty() && items.isEmpty()) {
		return;
	} else if (!event->node().isEmpty()) {
		node = event->node();
		factory = findFactory(QStringRef(&node));
	} else {
		factory = findFactory(items.at(0)->payloadType());
		node = factory ? factory->features().value(0) : QString();
	}
	if (!factory || node.isEmpty()) {
		Logger::warning() << "Invalid stanza extension at PubSub::Event";
		return;
	}
	writer->writeStartElement(QLatin1String("event"));
	writer->writeDefaultNamespace(NS_EVENT);
	writer->writeStartElement(QLatin1String("items"));
	writer->writeAttribute(QLatin1String("node"), node);
	for (int i = 0; i < items.size(); i++) {
		const Payload::Ptr &entity = items.at(i);
		if (entity->payloadType() != factory->payloadType())
			continue;
		writer->writeStartElement(QLatin1String("item"));
		factory->serialize(entity.data(), writer);
		writer->writeEndElement();
	}
	writer->writeEndElement();
	writer->writeEndElement();
}

Payload::Ptr EventFactory::createPayload()
{
	return Payload::Ptr(m_event.take());
}

AbstractPayloadFactory *EventFactory::findFactory(const QStringRef &node)
{
	for (int i = 0; i < m_factories.size(); i++) {
		if (m_factories.at(i)->features().value(0) == node)
			return m_factories.at(i);
	}
	return 0;
}

AbstractPayloadFactory *EventFactory::findFactory(int type)
{
	for (int i = 0; i < m_factories.size(); i++) {
		if (m_factories.at(i)->payloadType() == type)
			return m_factories.at(i);
	}
	return 0;
}
}
}
