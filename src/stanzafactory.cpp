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
	m_depth = 0;
}

StanzaFactory::~StanzaFactory()
{
}

void StanzaFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	while (!m_stanza->tokens.isEmpty() && m_stanza->tokens.last()->type == StanzaPrivate::Token::Characters)
		delete m_stanza->tokens.takeLast();
	StanzaPrivate::StartToken *token = new StanzaPrivate::StartToken;
	token->name = name.appendTo(&m_stanza->buffer);
	token->uri = uri.appendTo(&m_stanza->buffer);
	token->attributes = attributes;
	m_stanza->tokens << token;

	if (m_depth == 1) {
		m_stanza->from = attributes.value(QLatin1String("from")).toString();
		m_stanza->to = attributes.value(QLatin1String("to")).toString();
		m_stanza->id = attributes.value(QLatin1String("id")).toString();
	} else if (m_depth == 2) {
		foreach (AbstractPayloadFactory *factory, ClientPrivate::get(m_client)->factories) {
			if (factory->canParse(name, uri, attributes))
				m_parsers.append(factory);
		}
	}
	for (int i = 0; i < m_parsers.size(); i++)
		m_parsers.at(i)->handleStartElement(name, uri, attributes);
}

void StanzaFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	for (int i = 0; i < m_parsers.size(); i++)
		m_parsers.at(i)->handleEndElement(name, uri);
	m_stanza->tokens << new StanzaPrivate::EndToken;
	if (m_depth == 2) {
		for (int i = 0; i < m_parsers.size(); i++) {
			AbstractPayloadFactory *parser = static_cast<AbstractPayloadFactory*>(m_parsers.at(i));
			Payload::Ptr payload = parser->createPayload();
			if (payload.isNull())
				qFatal("Payload is null from %s", Payload::payloadName(parser->payloadType()));
			m_stanza->extensions.insert(payload->payloadType(), payload);
		}
		m_parsers.clear();
	}
}

void StanzaFactory::handleCharacterData(const QStringRef &text)
{
	for (int i = 0; i < m_parsers.size(); i++)
		m_parsers.at(i)->handleCharacterData(text);
	if (!m_stanza->tokens.isEmpty() && m_stanza->tokens.last()->type == StanzaPrivate::Token::EndElement)
		return;
	StanzaPrivate::CharactersToken *token = new StanzaPrivate::CharactersToken;
	token->text = text.appendTo(&m_stanza->buffer);
	m_stanza->tokens << token;
}

void StanzaFactory::serialize(Stanza *stanza, QXmlStreamWriter *writer)
{
	StanzaPrivate *p = StanzaPrivate::get(*stanza);
	QString namespaceUri = QLatin1String("jabber:client");
	QStack<QStringRef> uries;
	for (int i = 0; i < p->tokens.size(); ++i) {
		StanzaPrivate::Token * const token = p->tokens.at(i);
		if (token->type == StanzaPrivate::Token::StartElement) {
			StanzaPrivate::StartToken * const startToken = static_cast<StanzaPrivate::StartToken*>(token);
			writer->writeStartElement(startToken->name.toString());
			QStringRef currentUri = (i == 0) ? QStringRef(&namespaceUri) : startToken->uri;
			if (uries.isEmpty() || uries.top() != currentUri)
				writer->writeDefaultNamespace(startToken->uri.toString());
			uries.push(currentUri);
			writer->writeAttributes(startToken->attributes);
		} else if (token->type == StanzaPrivate::Token::Characters) {
			StanzaPrivate::CharactersToken * const charachtersToken = static_cast<StanzaPrivate::CharactersToken*>(token);
			writer->writeCharacters(charachtersToken->text.toString());
		} else if (token->type == StanzaPrivate::Token::EndElement) {
			writer->writeEndElement();
			uries.pop();
		} else {
			Q_ASSERT(!"Unknown token type");
		}
	}
}

void StanzaFactory::writeAttributes(Stanza *stanza, QXmlStreamWriter *writer)
{
	StanzaPrivate *p = StanzaPrivate::get(*stanza);
	if (p->from.isValid())
		writer->writeAttribute(QLatin1String("from"), p->from);
	if (p->to.isValid())
		writer->writeAttribute(QLatin1String("to"), p->to);
	if (!p->id.isEmpty())
		writer->writeAttribute(QLatin1String("id"), p->id);
	writer->writeDefaultNamespace(QLatin1String("jabber:client"));
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
