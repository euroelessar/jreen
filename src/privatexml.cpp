/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "privatexml_p.h"
#include "client_p.h"
#include <QXmlStreamWriter>
#define NS_PRIVATE_XML QLatin1String("jabber:iq:private")

namespace Jreen
{

PrivateXmlQueryFactory::PrivateXmlQueryFactory(Client *client)
    : m_depth(0), m_factory(0), m_client(ClientPrivate::get(client)), m_node(0)
{
}

PrivateXmlQueryFactory::~PrivateXmlQueryFactory()
{

}

QStringList PrivateXmlQueryFactory::features() const
{
	return QStringList(NS_PRIVATE_XML);
}

bool PrivateXmlQueryFactory::canParse(const QStringRef &name, const QStringRef &uri,
									  const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("query") && uri == NS_PRIVATE_XML;
}

void PrivateXmlQueryFactory::handleStartElement(const QStringRef &name,
												const QStringRef &uri,
												const QXmlStreamAttributes &attributes)
{
	m_depth++;
	if (m_depth == 1) {
		m_node.clear();
	} else if (m_depth == 2) {
		foreach (m_factory, m_client->factoriesByUri.values(uri.toString())) {
			if (m_factory->canParse(name, uri, attributes))
				break;
			else
				m_factory = 0;
		}
	}
	if (m_factory)
		m_factory->handleStartElement(name, uri, attributes);
}

void PrivateXmlQueryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	if (m_factory) {
		m_factory->handleEndElement(name, uri);
		if (m_depth == 2) {
			m_node = m_factory->createPayload();
			m_factory = 0;
		}
	}
	m_depth--;
}

void PrivateXmlQueryFactory::handleCharacterData(const QStringRef &text)
{
	if (m_factory)
		m_factory->handleCharacterData(text);
}

void PrivateXmlQueryFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	PrivateXmlQuery *query = se_cast<PrivateXmlQuery*>(extension);
	writer->writeStartElement(QLatin1String("query"));
	writer->writeDefaultNamespace(NS_PRIVATE_XML);
	if (query->type() == PrivateXmlQuery::Get) {
		writer->writeEmptyElement(query->name());
		writer->writeDefaultNamespace(query->namespaceURI());
	} else if (query->xml()) {
		Payload::Ptr node = query->xml();
		AbstractPayloadFactory *factory = m_client->factories.value(node->payloadType());
		if (factory)
			factory->serialize(node.data(), writer);
	}
	writer->writeEndElement();
}

Payload::Ptr PrivateXmlQueryFactory::createPayload()
{
	Payload::Ptr node;
	m_node.swap(node);
	return Payload::Ptr(new PrivateXmlQuery(node));
}

PrivateXml::PrivateXml(Client *client) : QObject(client), d_ptr(new PrivateXmlPrivate)
{
	Q_D(PrivateXml);
	d->client = client;
}

PrivateXml::~PrivateXml()
{
}

void PrivateXml::request(const QString &name, const QString &xmlns, QObject *handler, const char *member)
{
	Q_D(PrivateXml);
	QString id = d->client->getID();
	IQ iq(IQ::Get, JID(), id);
	iq.addExtension(new PrivateXmlQuery(name, xmlns));
	d->tracks.insert(id, new PrivateXmlTrack(handler, member));
	d->client->send(iq, this, SLOT(handleIQ(Jreen::IQ,int)), Request);
}

void PrivateXml::store(const Payload::Ptr &node, QObject *handler, const char *member)
{
	Q_D(PrivateXml);
	QString id = d->client->getID();
	IQ iq(IQ::Set, JID(), id);
	iq.addExtension(new PrivateXmlQuery(node));
	d->tracks.insert(id, new PrivateXmlTrack(handler, member));
	d->client->send(iq, this, SLOT(handleIQ(Jreen::IQ,int)), Store);
}

void PrivateXml::handleIQ(const IQ &iq, int context)
{
	Q_D(PrivateXml);
	PrivateXmlTrack *track = d->tracks.take(iq.id());
	if(!track)
		return;
	const QSharedPointer<Error> error = iq.payload<Error>();
	const QSharedPointer<PrivateXmlQuery> query = iq.payload<PrivateXmlQuery>();
	bool is_error = !query;
	if(query) {
		if(iq.subtype() == IQ::Result)
			track->resultReady(query->xml(), context == Store ? StoreOk : RequestOk, error);
		else if(iq.subtype() == IQ::Error)
			is_error = true;
	}
	if(is_error)
		track->resultReady(Payload::Ptr(), context == Store ? StoreError : RequestError, error);
	delete track;
}

} //namespace Jreen
