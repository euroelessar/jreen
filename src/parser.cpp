/*****************************************************************************
 *  parser.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "parser_p.h"
#include "message.h"
#include "subscription.h"
#include "presence.h"
#include <QDebug>

namespace jreen
{
Parser::Parser(Client *client) : d_ptr(new ParserPrivate)
{
	Q_D(Parser);
	d->client = ClientPrivate::get(client);
	d->reader = new QXmlStreamReader;
	d->state = WaitingForStanza;
	d->depth = 0;
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
		qDebug() << feature << feature->isActivatable();
		if (feature->isActivatable()) {
			d->client->current_stream_feature = feature;
			feature->activate();
			break;
		}
	}
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
		foreach (AbstractStanzaExtensionFactory *factory, d->client->factories) {
			if (factory->canParse(name, uri, attributes))
				d->parsers.append(factory);
		}
	}
	foreach (XmlStreamParser *parser, d->parsers)
		parser->handleStartElement(name, uri, attributes);
	//				qDebug() << d->reader->tokenString() << d->depth << name;
	d->depth++;
}

void Parser::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_D(Parser);
	d->depth--;
	for (int i = 0; i < d->parsers.size(); i++) {
		XmlStreamParser *parser = d->parsers.at(i);
		parser->handleEndElement(name, uri);
		if (d->depth == 2 && d->state == ReadStanza && i > d->parsersCount.at(1)) {
			StanzaExtension::Ptr se;
			se = static_cast<AbstractStanzaExtensionFactory*>(parser)->createExtension();
			d->extensions.append(se);
		}
	}
	if (d->depth == 1) {
		if (d->state == ReadFeatures) {
			d->client->current_stream_feature = 0;
			activateFeature();
		} else if (d->state == ReadStanza) {
			StanzaFactory *factory = static_cast<StanzaFactory*>(d->parsers.top());
			Stanza::Ptr stanza = factory->createStanza();
			foreach (const StanzaExtension::Ptr &se, d->extensions)
				stanza->addExtension(se);
			d->client->handleStanza(stanza);
			d->extensions.clear();
		}
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

void Parser::appendData(const QByteArray &a)
{
	Q_D(Parser);
	d->reader->addData(a);
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
	}
}
}
