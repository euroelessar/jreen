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

void Parser::appendData(const QByteArray &a)
{
	Q_D(Parser);
	d->reader->addData(a);
	while (d->reader->readNext() > QXmlStreamReader::Invalid) {
		switch(d->reader->tokenType()) {
		case QXmlStreamReader::StartElement:
			d->parsersCount.push(d->parsers.count());
			if (d->depth == 1) {
				if(d->reader->name() == QLatin1String("features")) {
					d->state = ReadFeatures;
				} else {
					foreach (StanzaFactory *factory, d->client->stanzas) {
						if (d->canParse(factory))
							d->parsers << factory;
					}
					if (d->parsers.isEmpty()) {
						foreach (StreamFeature *feature, d->client->features) {
							if (d->canParse(feature))
								d->parsers.append(feature);
						}
						d->state = ReadCustom;
					} else {
						d->state = ReadStanza;
					}
				}
			} else if (d->state == ReadFeatures && d->depth == 2) {
				foreach (StreamFeature *feature, d->client->features) {
					if (d->canParse(feature))
						d->parsers.append(feature);
				}
			} else if (d->state == ReadStanza && d->depth == 2) {
				foreach (AbstractStanzaExtensionFactory *factory, d->client->factories) {
					if (d->canParse(factory))
						d->parsers.append(factory);
				}
			}
			foreach (XmlStreamParser *parser, d->parsers)
				parser->handleStartElement(d->reader->name(), d->reader->namespaceUri(), d->reader->attributes());
			//				qDebug() << d->reader->tokenString() << d->depth << d->reader->name();
			d->depth++;
			break;
		case QXmlStreamReader::EndElement:
			d->depth--;
			for (int i = 0; i < d->parsers.size(); i++) {
				XmlStreamParser *parser = d->parsers.at(i);
				parser->handleEndElement(d->reader->name(), d->reader->namespaceUri());
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
			//				qDebug() << d->reader->tokenString() << d->depth << d->reader->name();
			break;
		case QXmlStreamReader::Characters:
			foreach (XmlStreamParser *parser, d->parsers)
				parser->handleCharacterData(d->reader->text());
			//				qDebug() << d->reader->tokenString() << d->reader->text();
			break;
		default:
			//				qDebug() << d->reader->tokenString();
			break;
		}
	}
}
}
