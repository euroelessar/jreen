/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "pubsubpublishfactory_p.h"

#include "pubsubeventfactory_p.h"
#include <QXmlStreamWriter>
#include <QStringList>

#define NS_PUBSUB QLatin1String("http://jabber.org/protocol/pubsub")

namespace Jreen
{
namespace PubSub
{
PublishFactory::PublishFactory(QList<AbstractStanzaExtensionFactory*> &factories) : m_factories(factories)
{
	m_depth = 0;
	m_state = AtNowhere;
}

PublishFactory::~PublishFactory()
{
}

QStringList PublishFactory::features() const
{
	return QStringList(NS_PUBSUB);
}

bool PublishFactory::canParse(const QStringRef &name, const QStringRef &uri,
							  const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("pubsub") && uri == NS_PUBSUB;
}

void PublishFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
										const QXmlStreamAttributes &attributes)
{
	m_depth++;
	if (m_depth == 1) {
		m_publish.reset(new Publish);
	} if (m_depth == 2 && name == QLatin1String("publish")) {
		findFactory(attributes.value(QLatin1String("node")));
		m_state = m_factory ? AtPublish : AtNowhere;
	} else if (m_depth == 3 && m_state == AtPublish && name == QLatin1String("item")) {
		m_state = AtItem;
	} else if (m_depth == 4 && m_state == AtItem && m_factory->canParse(name, uri, attributes)) {
		m_state = AtEntity;
	}
	if (m_state == AtEntity)
		m_factory->handleStartElement(name, uri, attributes);
}

void PublishFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	if (m_state == AtEntity)
		m_factory->handleEndElement(name, uri);
	if (m_depth == 2 && m_state == AtPublish) {
		m_state = AtNowhere;
	} else if (m_depth == 3 && m_state == AtItem) {
		m_state = AtPublish;
	} else if (m_depth == 4 && m_state == AtEntity) {
		m_state = AtItem;
		m_publish->items << m_factory->createExtension();
	}
	m_depth--;
}

void PublishFactory::handleCharacterData(const QStringRef &text)
{
	if (m_state == AtEntity)
		m_factory->handleCharacterData(text);
}

void PublishFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	Publish *publish = se_cast<Publish*>(extension);
	AbstractStanzaExtensionFactory *factory = 0;
	QString node;
	const QList<StanzaExtension::Ptr> &items = publish->items;
	if (publish->node.isEmpty() && items.isEmpty()) {
		return;
	} else if (!publish->node.isEmpty()) {
		node = publish->node;
		factory = findFactory(QStringRef(&node));
	} else {
		factory = findFactory(items.at(0)->extensionType());
		node = factory ? factory->features().value(0) : QString();
	}
	if (!factory || node.isEmpty()) {
		qWarning("Invalid stanza extension at PubSub::Publish");
		return;
	}
	writer->writeStartElement(QLatin1String("pusbsub"));
	writer->writeDefaultNamespace(NS_PUBSUB);
	writer->writeStartElement(QLatin1String("publish"));
	writer->writeAttribute(QLatin1String("node"), node);
	for (int i = 0; i < items.size(); i++) {
		const StanzaExtension::Ptr &entity = items.at(i);
		if (entity->extensionType() != factory->extensionType())
			continue;
		writer->writeStartElement(QLatin1String("item"));
		factory->serialize(entity.data(), writer);
		writer->writeEndElement();
	}
	writer->writeEndElement();
	if (publish->form) {
		writer->writeStartElement(QLatin1String("publish-options"));
		m_form.serialize(publish->form.data(), writer);
		writer->writeEndElement();
	}
	writer->writeEndElement();
}

StanzaExtension::Ptr PublishFactory::createExtension()
{
	return StanzaExtension::Ptr(m_publish.take());
}

AbstractStanzaExtensionFactory *PublishFactory::findFactory(const QStringRef &node)
{
	for (int i = 0; i < m_factories.size(); i++) {
		if (m_factories.at(i)->features().value(0) == node)
			return m_factories.at(i);
	}
	return 0;
}

AbstractStanzaExtensionFactory *PublishFactory::findFactory(int type)
{
	for (int i = 0; i < m_factories.size(); i++) {
		if (m_factories.at(i)->extensionType() == type)
			return m_factories.at(i);
	}
	return 0;
}
}
}
