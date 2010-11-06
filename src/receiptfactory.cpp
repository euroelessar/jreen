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

namespace jreen {

class ReceiptFactoryPrivate
{
public:
	Receipt::Type type;
};

ReceiptFactory::ReceiptFactory() : d_ptr(new ReceiptFactoryPrivate)
{
}

ReceiptFactory::~ReceiptFactory()
{

}

QStringList ReceiptFactory::features() const
{

}

bool ReceiptFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{

}

void ReceiptFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{

}

void ReceiptFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{

}

void ReceiptFactory::handleCharacterData(const QStringRef &text)
{

}

void ReceiptFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{

}

StanzaExtension::Ptr ReceiptFactory::createExtension()
{
	return StanzaExtension::Ptr(new Receipt(d_func()->type));
}

} // namespace jreen
