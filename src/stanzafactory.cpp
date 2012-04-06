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

#include "stanzafactory_p.h"
#include "client_p.h"
#include "logger.h"
#include <QStack>

namespace Jreen
{
StanzaFactory::StanzaFactory(Client *client) : m_client(client)
{
}

StanzaFactory::~StanzaFactory()
{
}

void StanzaFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	StanzaPrivate::StartToken *token = new StanzaPrivate::StartToken;
	token->name = QStringRef(&m_buffer, m_buffer.size(), name.size());
	m_buffer += name;
	token->uri = QStringRef(&m_buffer, m_buffer.size(), uri.size());
	m_buffer += uri;
	token->attributes = attributes;
	m_tokens << token;
}

void StanzaFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_tokens << new StanzaPrivate::EndToken;
}

void StanzaFactory::handleCharacterData(const QStringRef &text)
{
	StanzaPrivate::CharactersToken *token = new StanzaPrivate::CharactersToken;
	token->text = QStringRef(&m_buffer, m_buffer.size(), text.size());
	m_buffer += text;
}

void StanzaFactory::serialize(Stanza *stanza, QXmlStreamWriter *writer)
{
	StanzaPrivate *p = StanzaPrivate::get(*stanza);
	QStack<int> stack;
	for (int i = 0; i < p->tokens.size(); ++i) {
		StanzaPrivate::Token * const token = p->tokens.at(i);
		if (token->type == StanzaPrivate::Token::StartElement) {
			stack.push(i);
			StanzaPrivate::StartToken * const startToken = static_cast<StanzaPrivate::StartToken*>(token);
			writer->writeStartElement(startToken->name.toString());
			writer->writeDefaultNamespace(startToken->uri.toString());
			writer->writeAttributes(startToken->attributes);
		} else if (token->type == StanzaPrivate::Token::Characters) {
			StanzaPrivate::CharactersToken * const charachtersToken = static_cast<StanzaPrivate::CharactersToken*>(token);
			writer->writeCharacters(charachtersToken->text.toString());
		} else if (token->type == StanzaPrivate::Token::EndElement) {
			writer->writeEndElement();
		} else {
			Q_ASSERT(!"Unknown token type");
		}
	}
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
			Logger::debug() << "Invalid stanza extesion" << extension->payloadType();
	}
}

void StanzaFactory::writeEscapedString(const QString &str, QXmlStreamWriter *writer)
{
	writer->writeCharacters(str);
}

void StanzaFactory::writeLangMap(const QString &tag, const LangMap &map, QXmlStreamWriter *writer)
{
	foreach(QString lang,map.langs()) {
		writer->writeStartElement(tag);
		if(!lang.isEmpty())
			writer->writeAttribute(QLatin1String("xml:lang"),lang);
		writeEscapedString(map.value(lang), writer);
		writer->writeEndElement();
	}
}

}
