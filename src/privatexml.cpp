/****************************************************************************
 *  privatexml.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "privatexml_p.h"
#include <QXmlStreamWriter>
#define NS_PRIVATE_XML QLatin1String("jabber:iq:private")

namespace jreen
{

PrivateXml::QueryFactory::QueryFactory()
{

}

PrivateXml::QueryFactory::~QueryFactory()
{

}

QStringList PrivateXml::QueryFactory::features() const
{
	return QStringList(NS_PRIVATE_XML);
}

bool PrivateXml::QueryFactory::canParse(const QStringRef &name, const QStringRef &uri,
										const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("query") && uri == NS_PRIVATE_XML;
}

void PrivateXml::QueryFactory::handleStartElement(const QStringRef &name,
												  const QStringRef &uri,
												  const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	m_depth++;
}

void PrivateXml::QueryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth--;
}

void PrivateXml::QueryFactory::handleCharacterData(const QStringRef &text)
{
	if(m_depth == 2) {
		QDomDocument doc;
		doc.setContent(text.toString());
		m_xml = doc.firstChildElement();
	}
}

void PrivateXml::QueryFactory::serialize(StanzaExtension *extension,
										 QXmlStreamWriter *writer)
{
	PrivateXml::Query *query = se_cast<PrivateXml::Query*>(extension);
	writer->writeStartElement(QLatin1String("query"));
	writer->writeDefaultNamespace(NS_PRIVATE_XML);

	writer->writeStartElement(query->name());
	writer->writeDefaultNamespace(query->namespaceURI());
	writer->writeEndElement();

	writer->writeEndElement();
}

StanzaExtension::Ptr PrivateXml::QueryFactory::createExtension()
{
	return StanzaExtension::Ptr(new PrivateXml::Query(m_xml));
}

PrivateXml::PrivateXml(Client *client) : QObject(client), d_ptr(new PrivateXmlPrivate)
{
	Q_D(PrivateXml);
	d->client = client;
	d->client->registerStanzaExtension(new PrivateXml::QueryFactory);
}

PrivateXml::~PrivateXml()
{
}

void PrivateXml::request(const QString &name, const QString &xmlns, QObject *handler, const char *member)
{
	Q_D(PrivateXml);
	QString id = d->client->getID();
	IQ iq(IQ::Get, JID(), id);
	iq.addExtension(new Query(name, xmlns));
	d->tracks.insert(id, new PrivateXmlTrack(handler, member));
	d->client->send(iq, this, SLOT(handleIQ(IQ,int)), Request);
}

void PrivateXml::store(const QDomElement &node, QObject *handler, const char *member)
{
	Q_D(PrivateXml);
	QString id = d->client->getID();
	IQ iq(IQ::Get, JID(), id);
	iq.addExtension(new Query(node));
	d->tracks.insert(id, new PrivateXmlTrack(handler, member));
	d->client->send(iq, this, SLOT(handleIQ(IQ,int)), Store);
}

void PrivateXml::handleIQ(const IQ &iq, int context)
{
	Q_D(PrivateXml);
	PrivateXmlTrack *track = d->tracks.take(iq.id());
	if(!track)
		return;
	const QSharedPointer<Error> error = iq.findExtension<Error>();
	const QSharedPointer<Query> query = iq.findExtension<Query>();
	bool is_error = !query;
	if(query)
	{
		if(iq.subtype() == IQ::Result)
			track->newResult(query->xml(), context == Store ? StoreOk : RequestOk, error);
		else if(iq.subtype() == IQ::Error)
			is_error = true;
	}
	if(is_error)
		track->newResult(QDomElement(), context == Store ? StoreError : RequestError, error);
	delete track;
}

} //namespace jreen
