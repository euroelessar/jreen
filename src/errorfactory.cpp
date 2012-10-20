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

#include "errorfactory_p.h"
#include <QXmlStreamWriter>
#include <QStringList>
#include "jstrings.h"

#define NS_ERROR QLatin1String("urn:ietf:params:xml:ns:xmpp-stanzas")
//from rfc for messages and presence
//http://xmpp.org/rfcs/rfc3921.html#stanzas
#define NS_ERROR2 QLatin1String("jabber:client")


namespace Jreen {

const char *error_types[] = {"auth","cancel",
							 "continue","modify",
							 "wait"};

const char *error_conditions[] = {"bad-request","conflict",
								  "feature-not-implemented", "forbidden",
								  "gone", "internal-server-error",
								  "item-not-found", "jid-malformed",
								  "not-acceptable", "not-allowed",
								  "not-authorized", "not-modified",
								  "payment-required", "recipient-unavailable",
								  "redirect", "registration-required",
								  "remote-server-not-found", "remote-server-timeout",
								  "resource-constraint", "service-unavailable",
								  "subscription-required", "undefined-condition",
								  "unexpected-request", "unknown-sender"};

ErrorFactory::ErrorFactory()
	: m_state(AtCondition)
	, m_depth(0)
	, m_type(Error::UndefinedType)
	, m_condition(Error::Undefined)
{
}

bool ErrorFactory::canParse(const QStringRef& name, const QStringRef& uri, const QXmlStreamAttributes&)
{
	return name == QLatin1String("error") && (uri == NS_ERROR || uri == NS_ERROR2);
}

Payload::Ptr ErrorFactory::createPayload()
{
	return Payload::Ptr(new Error(m_type, m_condition, LangMap(m_text)));
}

QStringList ErrorFactory::features() const
{
	return QStringList(NS_ERROR);
}

void ErrorFactory::handleStartElement(const QStringRef& name, const QStringRef& uri, const QXmlStreamAttributes& attributes)
{
	Q_UNUSED(uri);
	m_depth++;
	if (m_depth == 1) {
		QStringRef subtype = attributes.value(QLatin1String("type"));
		m_type = strToEnum<Error::Type>(subtype,error_types);
		m_text.clear();
	} else if(m_depth == 2) {
		if(name == QLatin1String("text"))
			m_state = AtText;
		else {
			m_condition = strToEnum<Error::Condition>(name,error_conditions);
			m_state = AtCondition;
		}
	}
}

void ErrorFactory::handleCharacterData(const QStringRef& text)
{
	if (m_state == AtText)
		m_text = text.toString();
}
void ErrorFactory::handleEndElement(const QStringRef& name, const QStringRef& uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth--;
}

void ErrorFactory::serialize(Payload* obj, QXmlStreamWriter* writer)
{
	Error *error = se_cast<Error*>(obj);
	if(error->type() == Error::UndefinedType || error->condition() == Error::Undefined)
		return;
	writer->writeStartElement(QLatin1String("error"));
	writer->writeAttribute(QLatin1String("type"),enumToStr(error->type(),error_types));
	writer->writeEmptyElement(enumToStr(error->condition(),error_conditions));
	writer->writeDefaultNamespace(NS_ERROR);
	writer->writeEndElement();
}

ErrorFactory::~ErrorFactory()
{

}


} // namespace Jreen
