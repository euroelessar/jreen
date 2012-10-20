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
#include "logger.h"
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
}

static Client::Feature convertToFeature(int type)
{
	switch (type) {
	case StreamFeature::CompressionLayer:
		return Client::Compression;
	case StreamFeature::SecurityLayer:
		return Client::Encryption;
	case StreamFeature::SASL:
	case StreamFeature::SimpleAuthorization:
		return Client::Authorization;
	default:
		return Client::InvalidFeature;
	}
}

static Client::DisconnectReason convertToReason(Client::Feature feature)
{
	switch (feature) {
	case Client::Encryption:
		return Client::NoEncryptionSupport;
	case Client::Compression:
		return Client::NoCompressionSupport;
	case Client::Authorization:
		return Client::NoAuthorizationSupport;
	default:
		return Client::NoSupportedFeature;
	}
}

static bool checkFeature(ClientPrivate *client, Client::Feature feature)
{
	if (client->configs[feature] == Client::Force
	        && !(client->usedFeatures & (1 << feature))) {
		client->emitDisconnected(convertToReason(feature));
		return false;
	}
	return true;
}

void Parser::activateFeature()
{
	Q_D(Parser);
	int i = d->client->features.indexOf(d->client->current_stream_feature) + 1;
	d->client->current_stream_feature = 0;
	bool foundAny = false;
	for (; i < d->client->features.size(); i++) {
		StreamFeature *feature = d->client->features.at(i);
		if (!feature->isActivatable())
			continue;
		Client::Feature clientFeature = convertToFeature(feature->type());
		Client::FeatureConfig config = d->client->configs.value(clientFeature, Client::Auto);
		if (config == Client::Disable)
			continue;
		if (clientFeature == Client::InvalidFeature
		        && (!checkFeature(d->client, Client::Encryption)
		            || !checkFeature(d->client, Client::Compression)
		            || !checkFeature(d->client, Client::Authorization))) {
			return;
		}
		if (clientFeature == Client::Authorization
		        && !checkFeature(d->client, Client::Encryption)) {
			return;
		}
		d->client->current_stream_feature = feature;
		foundAny = true;
		feature->activate();
		if (clientFeature != Client::InvalidFeature)
			d->client->usedFeatures |= (1 << clientFeature);
		break;
	}
	if (!foundAny)
		d->client->emitDisconnected(Client::NoSupportedFeature);
}

#define XML_HEADER "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
#define HEADER XML_HEADER "<stream:stream xmlns:stream=\"http://etherx.jabber.org/streams\" xmlns=\"jabber:client\">"

QByteArray Parser::nextPart(QByteArray &data, bool first, bool *needMoreData)
{
	QByteArray result;
	*needMoreData = false;
	if (first) {
		first = false;
		int begin = data.indexOf('>');
		if (begin == -1) {
			*needMoreData = true;
			return result;
		}
		if (data[begin - 1] != '?') {
			result += XML_HEADER;
			begin = -1;
		}
		int end = data.indexOf('>', begin + 1) + 1;
		if (end <= 0) {
			*needMoreData = true;
			return QByteArray();
		}
		result += data.left(end);
		data.remove(0, end);
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
	}
	foreach (XmlStreamParser *parser, d->parsers)
		parser->handleStartElement(name, uri, attributes);
	//				Logger::debug() << d->reader->tokenString() << d->depth << name;
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
#ifdef PARSER_DEBUG_SPEED
			d->parsingTime += counter.elapsed();
			counter.restart();
#endif
			d->client->handleStanza(stanza);
#ifdef PARSER_DEBUG_SPEED
			d->stanzaLogicTime[factory->stanzaType()] += counter.elapsed();
#endif
		}
#ifdef PARSER_DEBUG_SPEED
		d->totalParsingTime += d->parsingTime;
		int logicTime = counter.elapsed();
		d->totalLogicTime += logicTime;
		Logger::debug() << "Total parsing time:" << d->totalParsingTime << "ms";
		Logger::debug() << "Parsing time:" << d->parsingTime << "ms";
		Logger::debug() << "Total logic time:" << d->totalLogicTime << "ms";
		Logger::debug() << "Total IQ logic time:" << d->stanzaLogicTime[0] << "ms";
		Logger::debug() << "Total Presence logic time:" << d->stanzaLogicTime[1] << "ms";
		Logger::debug() << "Total Message logic time:" << d->stanzaLogicTime[2] << "ms";
		Logger::debug() << "Logic time:" << logicTime << "ms";
#endif
		d->state = WaitingForStanza;
	} else if (d->depth == 0) {
	}
	d->parsers.resize(d->parsersCount.pop());
	//				Logger::debug() << d->reader->tokenString() << d->depth << name;
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
			Logger::warning() << "---------------------------------";
			Logger::warning() << "Broken stanza (" << d->nullReader.errorString() << ")";
			Logger::warning() << result;
			Logger::warning() << "---------------------------------";
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
