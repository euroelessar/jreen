#include "delayeddeliveryfactory.h"
#include <QStringList>
#include <QXmlStreamReader>
#include "util.h"


#define NS_DELAY "urn:xmpp:delay"
#define NS_DELAY_DEPRECATED "jabber:x:delay"

namespace Jreen {

class DelayedDeliveryFactoryPrivate
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

void DelayedDeliveryFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	DelayedDelivery *delivery = se_cast<DelayedDelivery*>(extension);
	if (!delivery->dateTime().isValid())
		return;
	writer->writeStartElement(QLatin1String("delay"));
	writer->writeAttribute(QLatin1String("stamp"), Util::toStamp(delivery->dateTime()));
	writer->writeDefaultNamespace(NS_DELAY);
	writer->writeAttribute(QLatin1String("from"), delivery->from());
	writer->writeCharacters(delivery->reason());
	writer->writeEndElement();
}

StanzaExtension::Ptr DelayedDeliveryFactory::createExtension()
{
	Q_D(DelayedDeliveryFactory);
	return StanzaExtension::Ptr(new DelayedDelivery(d->from,d->dateTime,d->reason));
}

} // namespace Jreen
