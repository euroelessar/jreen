/*****************************************************************************
 *  receiptfactory.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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
#include "receiptfactory.h"
#include <QStringList>
#include <QXmlStreamReader>
#include <QMap>

#define NS_RECEIPT QLatin1String("urn:xmpp:receipts")

namespace jreen {

class ReceiptFactoryPrivate
{
public:
	Receipt::Type type;
	QString id;
};

QMap<int,QString> typeStrings()
{
	QMap<int,QString> map;
	map.insert(Receipt::Received,QLatin1String("received"));
	map.insert(Receipt::Request,QLatin1String("request"));
	return map;
}

ReceiptFactory::ReceiptFactory() : d_ptr(new ReceiptFactoryPrivate)
{
}

ReceiptFactory::~ReceiptFactory()
{

}

QStringList ReceiptFactory::features() const
{
	return QStringList(NS_RECEIPT);
}

bool ReceiptFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return (typeStrings().key(name.toString(),-1) != -1) && uri == NS_RECEIPT;
}

void ReceiptFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_D(ReceiptFactory);
	Q_UNUSED(uri);
	d->id = attributes.value(QLatin1String("id")).toString();
	d->type = static_cast<Receipt::Type>(typeStrings().key(name.toString()));
}

void ReceiptFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
}

void ReceiptFactory::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

void ReceiptFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	Receipt *receipt = se_cast<Receipt*>(extension);
	writer->writeStartElement(typeStrings().value(receipt->type()));
	writer->writeAttribute(QLatin1String("id"),receipt->id());
	writer->writeDefaultNamespace(NS_RECEIPT);
	writer->writeEndElement();
}

StanzaExtension::Ptr ReceiptFactory::createExtension()
{
	Q_D(ReceiptFactory);
	return StanzaExtension::Ptr(new Receipt(d->type,d->id));
}

} // namespace jreen
