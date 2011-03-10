#ifndef DELAYEDDELIVERYFACTORY_H
#define DELAYEDDELIVERYFACTORY_H
#include "delayeddelivery.h"

namespace Jreen
{

class DelayedDeliveryFactoryPrivate;
class DelayedDeliveryFactory : public StanzaExtensionFactory<DelayedDelivery>
{
	Q_DECLARE_PRIVATE(DelayedDeliveryFactory)
public:
	DelayedDeliveryFactory();
	virtual ~DelayedDeliveryFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
private:
	QScopedPointer<DelayedDeliveryFactoryPrivate> d_ptr;
};

}
#endif // DELAYEDDELIVERYFACTORY_H
