/****************************************************************************
**
** Jreen
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "stanzafactory_p.h"
#include "client_p.h"

namespace Jreen
{
StanzaFactory::StanzaFactory(Client *client) : m_client(client)
{
}

StanzaFactory::~StanzaFactory()
{
}

void StanzaFactory::parseAttributes(const QXmlStreamAttributes &attributes)
{
	m_from = attributes.value(QLatin1String("from")).toString();
	m_to = attributes.value(QLatin1String("to")).toString();
	m_id = attributes.value(QLatin1String("id")).toString();
}

void StanzaFactory::writeAttributes(Stanza *stanza, QXmlStreamWriter *writer)
{
	if(stanza->from().isValid())
		writer->writeAttribute(QLatin1String("from"), stanza->from());
	if(stanza->to().isValid())
		writer->writeAttribute(QLatin1String("to"), stanza->to());
	if(!stanza->id().isEmpty())
		writer->writeAttribute(QLatin1String("id"), stanza->id());
	writer->writeAttribute(QLatin1String("xmlns"), QLatin1String("jabber:client"));
}

void StanzaFactory::writePayloads(Stanza *stanza, QXmlStreamWriter *writer)
{
	//What is the difference between the writer from the p->writer
	ClientPrivate *p = ClientPrivate::get(m_client);
	foreach(QSharedPointer<Payload> extension, stanza->payloads()) {
		AbstractPayloadFactory *factory = p->factories.value(extension->payloadType(), 0);
		if(factory)
			factory->serialize(extension.data(),writer);
		else
			qDebug("Invalid stanza extesion %d", extension->payloadType());
	}
}

void StanzaFactory::writeLangMap(const QString &tag, const LangMap &map, QXmlStreamWriter *writer)
{
	foreach(QString lang,map.langs()) {
		writer->writeStartElement(tag);
		if(!lang.isEmpty())
			writer->writeAttribute(QLatin1String("xml:lang"),lang);
		writer->writeCharacters(map.value(lang));
		writer->writeEndElement();
	}
}

}
