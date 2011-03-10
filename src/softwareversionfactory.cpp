#include "softwareversionfactory.h"
#include "jstrings.h"
#include <QStringList>
#include <QXmlStreamWriter>
#include "jstrings.h"
#include "util.h"

#define NS_SOFTWAREVERSION QLatin1String("jabber:iq:version")

namespace Jreen {

using namespace Util;

static const char *query_strings[] = {"name","version","os"};

class SoftwareVersionFactoryPrivate
{
public:
	int depth;
	enum State {AtName,AtVersion,AtOS};
	QString name;
	QString version;
	QString os;
	State state;
};

SoftwareVersionFactory::SoftwareVersionFactory() : d_ptr(new SoftwareVersionFactoryPrivate)
{
	d_func()->depth = 0;
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
	Q_UNUSED(attributes);
	return name == QLatin1String("query") && uri == NS_SOFTWAREVERSION;
}

void SoftwareVersionFactory::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	Q_D(SoftwareVersionFactory);
	d->depth++;
	if (d->depth == 1) {
		d->name.clear();
		d->os.clear();
		d->version.clear();
	}
	if(d->depth == 2)
		d->state = strToEnum<SoftwareVersionFactoryPrivate::State>(name,query_strings);
}

void SoftwareVersionFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(uri);
	Q_UNUSED(name);
	Q_D(SoftwareVersionFactory);
	d->depth--;
}

void SoftwareVersionFactory::handleCharacterData(const QStringRef &text)
{
	Q_D(SoftwareVersionFactory);
	if(d->depth == 2) {
		QString name = text.toString();
		switch(d->state) {
		case SoftwareVersionFactoryPrivate::AtName:
			d->name = name;
			break;
		case SoftwareVersionFactoryPrivate::AtVersion:
			d->version = name;
			break;
		case SoftwareVersionFactoryPrivate::AtOS:
			d->os = name;
			break;
		}
	}
}

void SoftwareVersionFactory::serialize(StanzaExtension *extension, QXmlStreamWriter *writer)
{
	SoftwareVersion *softwareVersion = se_cast<SoftwareVersion*>(extension);
	writer->writeStartElement(QLatin1String("query"));
	writer->writeDefaultNamespace(NS_SOFTWAREVERSION);
	writeTextElement(writer,QLatin1String("name"),softwareVersion->name());
	writeTextElement(writer,QLatin1String("version"),softwareVersion->version());
	writeTextElement(writer,QLatin1String("os"),softwareVersion->os());
	writer->writeEndElement();
}

StanzaExtension::Ptr SoftwareVersionFactory::createExtension()
{
	Q_D(SoftwareVersionFactory);
	return StanzaExtension::Ptr(new SoftwareVersion(d->name,d->version,d->os));
}

} // namespace Jreen
