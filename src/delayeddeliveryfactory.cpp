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
#include "delayeddeliveryfactory_p.h"
#include <QStringList>
#include <QXmlStreamReader>
#include "util.h"


#define NS_DELAY QLatin1String("urn:xmpp:delay")
#define NS_DELAY_DEPRECATED QLatin1String("jabber:x:delay")

namespace Jreen {

class JREEN_AUTOTEST_EXPORT DelayedDeliveryFactoryPrivate
{
public:
	JID from;
	QDateTime dateTime;
	QString reason;
};

DelayedDeliveryFactory::DelayedDeliveryFactory() : d_ptr(new DelayedDeliveryFactoryPrivate)
{
}

DelayedDeliveryFactory::~DelayedDeliveryFactory()
{

}

QStringList DelayedDeliveryFactory::features() const
{
	return QStringList(NS_DELAY);
}

bool DelayedDeliveryFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	return (name == QLatin1String("delay") && uri == NS_DELAY)
			|| (name == QLatin1String("x") && uri == NS_DELAY_DEPRECATED);
	Q_UNUSED(attributes);
}

void DelayedDeliveryFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_D(DelayedDeliveryFactory);
	Q_UNUSED(name);
	Q_UNUSED(uri);
	d->from = attributes.value(QLatin1String("from")).toString();
	d->dateTime = Util::fromStamp(attributes.value("stamp").toString());
}

void DelayedDeliveryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
}

void DelayedDeliveryFactory::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

void DelayedDeliveryFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	DelayedDelivery *delivery = se_cast<DelayedDelivery*>(extension);
	if (!delivery->dateTime().isValid())
		return;
	writer->writeStartElement(QLatin1String("delay"));
	writer->writeAttribute(QLatin1String("stamp"), Util::toStamp(delivery->dateTime()));
	writer->writeDefaultNamespace(NS_DELAY);
	if (delivery->from().isValid())
		writer->writeAttribute(QLatin1String("from"), delivery->from());
	writer->writeCharacters(delivery->reason());
	writer->writeEndElement();
}

Payload::Ptr DelayedDeliveryFactory::createPayload()
{
	Q_D(DelayedDeliveryFactory);
	return Payload::Ptr(new DelayedDelivery(d->from,d->dateTime,d->reason));
}

} // namespace Jreen
