#include "delayeddeliveryfactory.h"
#include <QStringList>
#include <QXmlStreamReader>
#include "util.h"
#include "jstrings.h"

namespace jreen {

J_STRING(delay)
J_STRING(stamp)

class DelayedDeliveryFactoryPrivate
{
public:
	JID from;
	QDateTime dateTime;
	QString reason;
};

DelayedDeliveryFactory::DelayedDeliveryFactory()
{
}

QStringList DelayedDeliveryFactory::features() const
{

}

bool DelayedDeliveryFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{

}

void DelayedDeliveryFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{

}

void DelayedDeliveryFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{

}

void DelayedDeliveryFactory::handleCharacterData(const QStringRef &text)
{

}

void DelayedDeliveryFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	DelayedDelivery *delivery = se_cast<DelayedDelivery*>(extension);
	if (!delivery->dateTime().isValid())
		return;
	writer->writeStartElement(delay_str);
	writer->writeAttribute(stamp_str, Util::toStamp(delivery->dateTime()));
	writer->writeAttribute(ConstString::xmlns, ConstString::xmlns_delay);
	writer->writeAttribute(ConstString::from, delivery->from());
	writer->writeCharacters(delivery->reason());
	writer->writeEndElement();
}

StanzaExtension::Ptr DelayedDeliveryFactory::createExtension()
{
	Q_D(DelayedDeliveryFactory);
	return StanzaExtension::Ptr(new DelayedDelivery(d->from,d->dateTime,d->reason));
}

} // namespace jreen
