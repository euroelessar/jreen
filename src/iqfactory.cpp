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

#include "iqfactory_p.h"
#include "iq_p.h"
#include "logger.h"

namespace Jreen
{
IqFactory::IqFactory(Client *client) : StanzaFactory(client)
{
}

int IqFactory::stanzaType()
{
	return StanzaPrivate::StanzaIq;
}

Stanza::Ptr IqFactory::createStanza()
{
	return Stanza::Ptr(new IQ(*static_cast<IQPrivate*>(m_stanza.take())));
}

void IqFactory::serialize(Stanza *stanza, QXmlStreamWriter *writer)
{
	if (!StanzaPrivate::get(*stanza)->tokens.isEmpty()) {
		StanzaFactory::serialize(stanza, writer);
		return;
	}
	IQ *iq = static_cast<IQ*>(stanza);
	if (iq->subtype() == IQ::Invalid)
		return;
	writer->writeStartElement(QLatin1String("iq"));
	writeAttributes(stanza, writer);
	QString type;
	switch (iq->subtype()) {
	case IQ::Get:
		type = QLatin1String("get");
		break;
	case IQ::Set:
		type = QLatin1String("set");
		break;
	case IQ::Result:
		type = QLatin1String("result");
		break;
	case IQ::Error:
		type = QLatin1String("error");
		break;
	default:
		break;
	}
	writer->writeAttribute(QLatin1String("type"), type);
	writePayloads(stanza, writer);
	writer->writeEndElement();
}

bool IqFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	return name == QLatin1String("iq");
}

void IqFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	m_depth++;
	if (m_depth == 1)
		m_stanza.reset(new IQPrivate);
	StanzaFactory::handleStartElement(name, uri, attributes);
	if (m_depth == 1) {
		IQPrivate *p = static_cast<IQPrivate*>(m_stanza.data());
		QStringRef type = attributes.value(QLatin1String("type"));
		if (type == QLatin1String("get"))
			p->subtype = IQ::Get;
		else if (type == QLatin1String("set"))
			p->subtype = IQ::Set;
		else if (type == QLatin1String("result"))
			p->subtype = IQ::Result;
		else if (type == QLatin1String("error"))
			p->subtype = IQ::Error;
		else
			p->subtype = IQ::Invalid;
	}
}

void IqFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	StanzaFactory::handleEndElement(name, uri);
	m_depth--;
}

void IqFactory::handleCharacterData(const QStringRef &name)
{
	StanzaFactory::handleCharacterData(name);
}

}
