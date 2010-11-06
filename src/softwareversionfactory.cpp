#include "softwareversionfactory.h"
#include "jstrings.h"
#include <QStringList>
#include <QXmlStreamWriter>

#define NS_SOFTWAREVERSION QLatin1String("jabber:iq:version")

namespace jreen {

class SoftwareVersionFactoryPrivate
{
public:
	QString name;
	QString version;
	QString os;
};

SoftwareVersionFactory::SoftwareVersionFactory() : d_ptr(new SoftwareVersionFactoryPrivate)
{

}

SoftwareVersionFactory::~SoftwareVersionFactory()
{

}

QStringList SoftwareVersionFactory::features() const
{
	return QStringList(NS_SOFTWAREVERSION);
}

bool SoftwareVersionFactory::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	return name == QLatin1String("query") && uri == NS_SOFTWAREVERSION;
}

void SoftwareVersionFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{

}

void SoftwareVersionFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{

}

void SoftwareVersionFactory::handleCharacterData(const QStringRef &text)
{

}

void SoftwareVersionFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	SoftwareVersion *softwareVersion = se_cast<SoftwareVersion*>(extension);
	writer->writeStartElement(NS_SOFTWAREVERSION,ConstString::query);
	writer->writeTextElement(ConstString::name,softwareVersion->name());
	writer->writeTextElement(QLatin1String("version"),softwareVersion->version());
	writer->writeTextElement(QLatin1String("os"),softwareVersion->os());
	writer->writeEndElement();
}

StanzaExtension::Ptr SoftwareVersionFactory::createExtension()
{
	Q_D(SoftwareVersionFactory);
	return StanzaExtension::Ptr(new SoftwareVersion(d->name,d->version,d->os));
}

} // namespace jreen
