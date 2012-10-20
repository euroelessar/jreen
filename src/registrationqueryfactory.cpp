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

#include "registrationqueryfactory_p.h"
#include "jstrings.h"

#define NS_REGISTER QLatin1String("jabber:iq:register")

namespace Jreen
{

const char *emptyNames[] = {
	"registered",
	"remove"
};

const char *valueNames[] = {
	"username",
	"nick",
	"password",
	"name",
	"first",
	"last",
	"email",
	"address",
	"city",
	"state",
	"zip",
	"phone",
	"url",
	"date"
	"misc",
	"text",
	"key"
};


RegistrationQueryFactory::RegistrationQueryFactory() : m_state(Nowhere), m_depth(0)
{
}

QStringList RegistrationQueryFactory::features() const
{
	return QStringList(); // << NS_REGISTER;
}

bool RegistrationQueryFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	m_state = Nowhere;
	m_depth = 0;
	m_query.reset(new RegistrationQuery);
	return name == QLatin1String("query") && uri == NS_REGISTER;
}

void RegistrationQueryFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	++m_depth;
	if (m_depth == 2) {
		if (m_formFactory.canParse(name, uri, attributes)) {
			m_state = AtForm;
		} else if (m_bobFactory.canParse(name, uri, attributes)) {
			m_state = AtBob;
		} else if (name == QLatin1String("instructions")) {
			m_state = AtInstructions;
		} else {
			RegistrationDataPrivate::Flag flag = strToFlag<RegistrationDataPrivate::Flag>(name, emptyNames);
			if (flag != -1) {
				m_query->data->flags |= flag;
				return;
			} 
			int index = strToEnum(name, valueNames);
			if (index != -1) {
				m_query->data->valuesFlags |= (1 << index);
				m_state = static_cast<State>(AtValue + index);
				return;
			}
		}
	}
	if (m_state == AtForm)
		m_formFactory.handleStartElement(name, uri, attributes);
	else if (m_state == AtBob)
		m_bobFactory.handleStartElement(name, uri, attributes);
}

void RegistrationQueryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	if (m_state == AtForm)
		m_formFactory.handleEndElement(name, uri);
	else if (m_state == AtBob)
		m_bobFactory.handleEndElement(name, uri);
	if (m_depth == 2) {
		if (m_state == AtForm)
			m_query->data->form = m_formFactory.createPayload().staticCast<DataForm>();
		else if (m_state == AtBob)
			m_query->data->bobs << m_bobFactory.createPayload().staticCast<BitsOfBinary>();
		m_state = Nowhere;
	}
	--m_depth;
}

void RegistrationQueryFactory::handleCharacterData(const QStringRef &text)
{
	if (m_state == AtForm)
		m_formFactory.handleCharacterData(text);
	else if (m_state == AtBob)
		m_bobFactory.handleCharacterData(text);
	else if (m_state == AtInstructions)
		m_query->data->instructions = text.toString();
	else if (m_state >= AtValue)
		m_query->data->values[m_state - AtValue] = text.toString();
}

void RegistrationQueryFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	RegistrationQuery *query = se_cast<RegistrationQuery*>(extension);
	writer->writeStartElement(QLatin1String("query"));
	writer->writeDefaultNamespace(NS_REGISTER);
	for (uint i = 0; i < sizeof(emptyNames) / sizeof(emptyNames[0]); ++i) {
		if (query->data->flags & (1 << i))
			writer->writeEmptyElement(QLatin1String(emptyNames[i]));
	}
	for (int i = 0; i < query->data->values.size(); ++i) {
		if (query->data->valuesFlags & (1 << i))
			writer->writeTextElement(QLatin1String(valueNames[i]), query->data->values[i]);
	}
	if (query->data->form)
		m_formFactory.serialize(query->data->form.data(), writer);
	writer->writeEndElement();
}

Payload::Ptr RegistrationQueryFactory::createPayload()
{
	return Payload::Ptr(m_query.take());
}

} // namespace Jreen
