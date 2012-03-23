/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "bitsofbinaryfactory_p.h"
#include <QUrl>
#include <QStringList>

namespace Jreen
{

#define NS_BOB QLatin1String("urn:xmpp:bob")

BitsOfBinaryFactory::BitsOfBinaryFactory() : m_depth(0)
{
}

QStringList BitsOfBinaryFactory::features() const
{
	return QStringList();
}

bool BitsOfBinaryFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	m_depth = 0;
	return name == QLatin1String("data") && uri == NS_BOB;
}

void BitsOfBinaryFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	++m_depth;
	
	if (m_depth == 1) {
		m_query.reset(new BitsOfBinary);
		m_query->setCid(QUrl(attributes.value(QLatin1String("cid")).toString()));
		QStringRef maxAgeString = attributes.value(QLatin1String("max-age"));
		bool ok = true;
		qint64 maxAge = maxAgeString.toString().toInt(&ok);
		m_query->setMaximumAge(ok ? maxAge : -1);
		m_query->setType(attributes.value(QLatin1String("type")).toString());
	}
}

void BitsOfBinaryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	--m_depth;
}

void BitsOfBinaryFactory::handleCharacterData(const QStringRef &text)
{
	if (m_depth == 1)
		m_query->setData(QByteArray::fromBase64(text.toString().toLatin1()));
}

void BitsOfBinaryFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	BitsOfBinary *query = payload_cast<BitsOfBinary*>(extension);
	writer->writeStartElement(QLatin1String("data"));
	writer->writeAttribute(QLatin1String("cid"), query->cid().toString());
	if (!query->type().isEmpty())
		writer->writeAttribute(QLatin1String("type"), query->type());
	if (query->maximumAge() >= 0)
		writer->writeAttribute(QLatin1String("max-age"), QString::number(query->maximumAge()));
	writer->writeDefaultNamespace(NS_BOB);
	writer->writeCharacters(QLatin1String(query->data().toBase64()));
	writer->writeEndElement();
}

Payload::Ptr BitsOfBinaryFactory::createPayload()
{
	return Payload::Ptr(m_query.take());
}

} // namespace Jreen
