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

#include "parser_p.h"
#include "message.h"
#include "subscription.h"
#include "presence.h"
#include <QCoreApplication>
#include <QDebug>
#ifdef PARSER_DEBUG_SPEED
#include <QTime>
#endif

Q_GLOBAL_STATIC_WITH_ARGS(QEvent::Type, parserHookEventId,
						  (static_cast<QEvent::Type>(QEvent::registerEventType())))

namespace Jreen
{
Parser::Parser(Client *client) : d_ptr(new ParserPrivate)
{
	Q_D(Parser);
	d->client = ClientPrivate::get(client);
	d->reader = new QXmlStreamReader;
	d->state = WaitingForStanza;
	d->depth = 0;
#ifdef PARSER_DEBUG_SPEED
	d->parsingTime = 0;
	d->totalParsingTime = 0;
	d->totalLogicTime = 0;
	qMemSet(d->stanzaLogicTime, 0, sizeof(d->stanzaLogicTime));
#endif
	d->atParsing = false;
}

Parser::~Parser()
{
	Q_D(Parser);
	delete d->reader;
}

void Parser::reset()
{
	Q_D(Parser);
	d->reader->clear();
	d->nullReader.clear();
	d->first = true;
	d->buffer.clear();
	d->state = WaitingForStanza;
	d->depth = 0;
	foreach (XmlStreamParser *parser, d->parsers)
		parser->handleEndElement(QStringRef(), QStringRef());
	d->parsers.clear();
	foreach (StreamFeature *feature, d->client->features)
		feature->reset();
	d->extensions.clear();
}

void Parser::activateFeature()
{
	Q_D(Parser);
	int i = d->client->features.indexOf(d->client->current_stream_feature) + 1;
	d->client->current_stream_feature = 0;
	for (; i < d->client->features.size(); i++) {
		StreamFeature *feature = d->client->features.at(i);
		if (feature->isActivatable()) {
			d->client->current_stream_feature = feature;
			feature->activate();
			break;
		}
	}
}

QByteArray Parser::nextPart(QByteArray &data, bool first, bool *needMoreData)
{
	QByteArray result;
	*needMoreData = false;
	if (first) {
		first = false;
		int begin = data.indexOf('>') + 1;
		int end = data.indexOf('>', begin) + 1;
		if (begin == -1 || end == -1) {
			*needMoreData = true;
			return result;
		}
		result = data.left(end);
		data.remove(0, result.size());
	} else {
		int depth = 0;
		int index = 0;
		while (true) {
			int start = data.indexOf('<', index);
			int end = data.indexOf('>', start);
			if (start == -1 || end == -1) {
				*needMoreData = true;
				return result;
			}
			index = end + 1;
			bool startTag = data[start + 1] != '/';
			if (startTag) {
				bool emptyTag = data[end - 1] == '/';
				depth += !emptyTag;
			} else {
				depth--;
			}
			if (depth == 0) {
				break;
			} else if (depth < 0) {
				qSwap(result, data);
				return result;
			}
		}
		result = data.left(index);
		data.remove(0, result.size());
	}
	return result;
}

bool Parser::canParse(const QStringRef &, const QStringRef &, const QXmlStreamAttributes &)
{
	return true;
}

void Parser::handleStartElement(const QStringRef &name, const QStringRef &uri,
								const QXmlStreamAttributes &attributes)
{
	Q_D(Parser);
	d->parsersCount.push(d->parsers.count());
#ifdef PARSER_DEBUG_SPEED
	if (d->depth == 1)
		d->parsingTime = 0;
	QTime counter;
	counter.start();
#endif
	if (d->depth == 1) {
		if(name == QLatin1String("features")) {
			d->state = ReadFeatures;
		} else {
			foreach (StanzaFactory *factory, d->client->stanzas) {
				if (factory->canParse(name, uri, attributes))
					d->parsers << factory;
			}
			if (d->parsers.isEmpty()) {
				foreach (StreamFeature *feature, d->client->features) {
					if (feature->canParse(name, uri, attributes))
						d->parsers.append(feature);
				}
				d->state = ReadCustom;
			} else {
				d->state = ReadStanza;
			}
		}
	} else if (d->state == ReadFeatures && d->depth == 2) {
		foreach (StreamFeature *feature, d->client->features) {
			if (feature->canParse(name, uri, attributes))
				d->parsers.append(feature);
		}
	} else if (d->state == ReadStanza && d->depth == 2) {
		foreach (AbstractPayloadFactory *factory, d->client->factories) {
			if (factory->canParse(name, uri, attributes))
				d->parsers.append(factory);
		}
	}
	foreach (XmlStreamParser *parser, d->parsers)
		parser->handleStartElement(name, uri, attributes);
	//				qDebug() << d->reader->tokenString() << d->depth << name;
#ifdef PARSER_DEBUG_SPEED
	d->parsingTime += counter.elapsed();
#endif
	d->depth++;
}

void Parser::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_D(Parser);
	d->depth--;
#ifdef PARSER_DEBUG_SPEED
	QTime counter;
	counter.start();
#endif
	for (int i = 0; i < d->parsers.size(); i++) {
		XmlStreamParser *parser = d->parsers.at(i);
		parser->handleEndElement(name, uri);
		if (d->depth == 2 && d->state == ReadStanza && i > d->parsersCount.at(1)) {
			Payload::Ptr se;
			se = static_cast<AbstractPayloadFactory*>(parser)->createPayload();
			d->extensions.append(se);
		}
	}
#ifdef PARSER_DEBUG_SPEED
	d->parsingTime += counter.elapsed();
#endif
	if (d->depth == 1) {
#ifdef PARSER_DEBUG_SPEED
		counter.restart();
#endif
		if (d->state == ReadFeatures) {
			d->client->current_stream_feature = 0;
			activateFeature();
		} else if (d->state == ReadStanza) {
			StanzaFactory *factory = static_cast<StanzaFactory*>(d->parsers.top());
			Stanza::Ptr stanza = factory->createStanza();
			foreach (const Payload::Ptr &se, d->extensions)
				stanza->addExtension(se);
#ifdef PARSER_DEBUG_SPEED
			d->parsingTime += counter.elapsed();
			counter.restart();
#endif
			d->client->handleStanza(stanza);
			d->extensions.clear();
#ifdef PARSER_DEBUG_SPEED
			d->stanzaLogicTime[factory->stanzaType()] += counter.elapsed();
#endif
		}
#ifdef PARSER_DEBUG_SPEED
		d->totalParsingTime += d->parsingTime;
		int logicTime = counter.elapsed();
		d->totalLogicTime += logicTime;
		qDebug("Total parsing time: %d ms", d->totalParsingTime);
		qDebug("Parsing time: %d ms", d->parsingTime);
		qDebug("Total logic time: %d ms", d->totalLogicTime);
		qDebug("Total IQ logic time: %d ms", d->stanzaLogicTime[0]);
		qDebug("Total Presence logic time: %d ms", d->stanzaLogicTime[1]);
		qDebug("Total Message logic time: %d ms", d->stanzaLogicTime[2]);
		qDebug("Logic time: %d ms", logicTime);
#endif
		d->state = WaitingForStanza;
	} else if (d->depth == 0) {
	}
	d->parsers.resize(d->parsersCount.pop());
	//				qDebug() << d->reader->tokenString() << d->depth << name;
}

void Parser::handleCharacterData(const QStringRef &text)
{
	Q_D(Parser);
	foreach (XmlStreamParser *parser, d->parsers)
		parser->handleCharacterData(text);
}

bool Parser::event(QEvent *ev)
{
	if (ev->type() == *parserHookEventId()) {
		parseData();
		return true;
	}
	return QObject::event(ev);
}

void Parser::appendData(const QByteArray &a)
{
	Q_D(Parser);
	d->buffer.append(a);
//	d->reader->addData(a);
	parseData();
}

#define HEADER "<?xml version=\"1.0\" encoding=\"UTF-8\"?><stream:stream xmlns:stream=\"http://etherx.jabber.org/streams\" xmlns=\"jabber:client\">"

void Parser::parseData()
{
	Q_D(Parser);
	if (d->atParsing)
		return;
	d->atParsing = true;
	bool needMoreData = false;
	while (!d->buffer.isEmpty()) {
		QByteArray result = nextPart(d->buffer, d->first, &needMoreData);
		if (needMoreData) {
			d->atParsing = false;
			return;
		}
		d->nullReader.addData(result);
		d->first = false;
		forever {
			QXmlStreamReader::TokenType type = d->nullReader.readNext();
			if (type == QXmlStreamReader::Invalid
			        || type == QXmlStreamReader::EndDocument) {
				break;
			}
		}
		if (d->nullReader.error() == QXmlStreamReader::NotWellFormedError) {
			qWarning() << "---------------------------------";
			qWarning() << "Broken stanza (" << d->nullReader.errorString() << ")";
			qWarning() << result;
			qWarning() << "---------------------------------";
			result.prepend("<!--");
			result.append("-->");
			foreach (XmlStreamHandler *handler, d->client->streamHandlers)
				handler->handleIncomingData(result.constData(), result.size());
			
			d->nullReader.clear();
			d->nullReader.addData(HEADER);
			continue;
		}
		d->reader->addData(result);
		foreach (XmlStreamHandler *handler, d->client->streamHandlers)
			handler->handleIncomingData(result.constData(), result.size());
		while (d->reader->readNext() > QXmlStreamReader::Invalid) {
			switch(d->reader->tokenType()) {
			case QXmlStreamReader::StartElement:
				handleStartElement(d->reader->name(), d->reader->namespaceUri(), d->reader->attributes());
				break;
			case QXmlStreamReader::EndElement:
				handleEndElement(d->reader->name(), d->reader->namespaceUri());
				break;
			case QXmlStreamReader::Characters:
				handleCharacterData(d->reader->text());
				break;
			default:
				break;
			}
#ifdef PARSER_SPLIT_STANZAS_EVENTS
			if (d->depth == 1 && d->reader->tokenType() == QXmlStreamReader::EndElement) {
				d->atParsing = false;
				qApp->postEvent(this, new QEvent(*parserHookEventId()));
				return;
			}
#endif
		}
	}
	d->atParsing = false;
}
}
