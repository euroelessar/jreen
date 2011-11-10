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

#include "iqfactory_p.h"
#include "iq_p.h"
#include <QDebug>

namespace Jreen
{
IqFactory::IqFactory(Client *client) : StanzaFactory(client)
{
	m_depth = 0;
}

int IqFactory::stanzaType()
{
	return StanzaPrivate::StanzaIq;
}

Stanza::Ptr IqFactory::createStanza()
{
	IQPrivate *p = new IQPrivate;
	p->from = m_from;
	p->to = m_to;
	p->id = m_id;
	p->subtype = m_type;
	return Stanza::Ptr(new IQ(*p));
}

void IqFactory::serialize(Stanza *stanza, QXmlStreamWriter *writer)
{
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
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth++;
	if (m_depth == 1) {
		parseAttributes(attributes);
		QStringRef type = attributes.value(QLatin1String("type"));
		if (type == QLatin1String("get"))
			m_type = IQ::Get;
		else if (type == QLatin1String("set"))
			m_type = IQ::Set;
		else if (type == QLatin1String("result"))
			m_type = IQ::Result;
		else if (type == QLatin1String("error"))
			m_type = IQ::Error;
		else
			m_type = IQ::Invalid;
	}
}

void IqFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	m_depth--;
	Q_UNUSED(name);
	Q_UNUSED(uri);
}

void IqFactory::handleCharacterData(const QStringRef &name)
{
	Q_UNUSED(name);
}

}
