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
#ifndef RECEIPTFACTORY_H
#define RECEIPTFACTORY_H
#include "receipt.h"

namespace Jreen {

class ReceiptFactoryPrivate;
class ReceiptFactory : public StanzaExtensionFactory<Receipt>
{
	Q_DECLARE_PRIVATE(ReceiptFactory)
public:
	ReceiptFactory();
	virtual ~ReceiptFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
private:
	QScopedPointer<ReceiptFactoryPrivate> d_ptr;
};

} // namespace Jreen

#endif // RECEIPTFACTORY_H
