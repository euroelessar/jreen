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
#include "softwareversionfactory_p.h"
#include "jstrings.h"
#include <QStringList>
#include <QXmlStreamWriter>
#include "jstrings.h"
#include "util.h"

#define NS_SOFTWAREVERSION QLatin1String("jabber:iq:version")

namespace Jreen {

using namespace Util;

static const char *query_strings[] = {"name","version","os"};

class JREEN_AUTOTEST_EXPORT SoftwareVersionFactoryPrivate
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

void SoftwareVersionFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	SoftwareVersion *softwareVersion = se_cast<SoftwareVersion*>(extension);
	writer->writeStartElement(QLatin1String("query"));
	writer->writeDefaultNamespace(NS_SOFTWAREVERSION);
	writeTextElement(writer,QLatin1String("name"),softwareVersion->name());
	writeTextElement(writer,QLatin1String("version"),softwareVersion->version());
	writeTextElement(writer,QLatin1String("os"),softwareVersion->os());
	writer->writeEndElement();
}

Payload::Ptr SoftwareVersionFactory::createPayload()
{
	Q_D(SoftwareVersionFactory);
	return Payload::Ptr(new SoftwareVersion(d->name,d->version,d->os));
}

} // namespace Jreen
