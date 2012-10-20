/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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
#include "receiptfactory_p.h"
#include <QStringList>
#include <QXmlStreamReader>
#include <QMap>
#include "jstrings.h"
#include "logger.h"

#define NS_RECEIPT QLatin1String("urn:xmpp:receipts")

namespace Jreen {

static const char *receipt_strings[] = {"request","received"};

class JREEN_AUTOTEST_EXPORT ReceiptFactoryPrivate
{
public:
	Receipt::Type type;
	QString id;
};

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
	return (strToEnum(name,receipt_strings) != -1) && uri == NS_RECEIPT;
}

void ReceiptFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_D(ReceiptFactory);
	Q_UNUSED(uri);
	d->id = attributes.value(QLatin1String("id")).toString();
	d->type = strToEnum<Receipt::Type>(name,receipt_strings);
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

void ReceiptFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	Receipt *receipt = se_cast<Receipt*>(extension);
	writer->writeStartElement(enumToStr(receipt->type(),receipt_strings));
	if(!receipt->id().isEmpty())
		writer->writeAttribute(QLatin1String("id"),receipt->id());
	writer->writeDefaultNamespace(NS_RECEIPT);
	writer->writeEndElement();
}

Payload::Ptr ReceiptFactory::createPayload()
{
	Q_D(ReceiptFactory);
	return Payload::Ptr(new Receipt(d->type,d->id));
}

} // namespace Jreen
