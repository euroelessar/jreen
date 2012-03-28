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

#include "capabilitiesfactory_p.h"
#include "disco.h"
#include <QStringList>
#include <QCryptographicHash>
#include <QXmlStreamWriter>
#include <QStringBuilder>
#define NS_CAPS QLatin1String("http://jabber.org/protocol/caps")

namespace Jreen {

QString CapabilitesFactory::verificationValue(Jreen::Disco *disco)
{
	QString s;
	QStringList sl;
	const Disco::IdentityList &identity_list = disco->identities();
	foreach(const Disco::Identity &i, identity_list) {
		sl << (i.category() % QLatin1Char('/')
		       % i.type() % QLatin1Char('/')
		       % i.lang() % QLatin1Char('/')
			   % i.name());
	}
	sl.sort();
	foreach(const QString &str, sl)
		s.append(str).append(QLatin1Char('<'));
	sl = disco->features().values();
	sl.sort();
	foreach(const QString &str, sl)
		s.append(str).append(QLatin1Char('<'));
	const DataForm *data = disco->form();
	if(data) {
		QString form_type;
		QMap<QString,QStringList> fields;
		for (int i = 0; i < data->fieldsCount(); i++) {
			DataFormField field = data->field(i);
			if(field.var() == QLatin1String("FORM_TYPE"))
				form_type = field.cast<DataFormFieldHidden>().value();
			else
				fields.insert(field.var(), QVariant(field.values()).toStringList());
		}	
		s.append(form_type).append(QLatin1Char('<'));

		QMap<QString,QStringList>::iterator it = fields.begin();
		for(; it != fields.end(); it++) {
			s.append(it.key()).append(QLatin1Char('<'));
			foreach(const QString &value, it.value())
				s.append(value).append(QLatin1Char('<'));
		}
	}
	return s;
}

QString CapabilitesFactory::hashValue(Disco *disco)
{
	const QString s = verificationValue(disco);
	return QString::fromLatin1(QCryptographicHash::hash(s.toUtf8(), QCryptographicHash::Sha1).toBase64());
}

CapabilitesFactory::CapabilitesFactory(Disco *disco)
	:	m_disco(disco)
{
}

CapabilitesFactory::~CapabilitesFactory()
{
}

QStringList CapabilitesFactory::features() const
{
	return QStringList(NS_CAPS);
}

bool CapabilitesFactory::canParse(const QStringRef &name, const QStringRef &uri,
								  const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("c") && uri == NS_CAPS;
}

void CapabilitesFactory::handleStartElement(const QStringRef &name, const QStringRef &uri,
											const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_node = attributes.value(QLatin1String("node")).toString();
	m_ver = attributes.value(QLatin1String("ver")).toString();
}

void CapabilitesFactory::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
}

void CapabilitesFactory::handleCharacterData(const QStringRef &text)
{
	Q_UNUSED(text);
}

void CapabilitesFactory::serialize(Payload *extension, QXmlStreamWriter *writer)
{
	Capabilities *caps = se_cast<Capabilities*>(extension);
	QString ver = caps->ver().isEmpty() ? hashValue(m_disco) : caps->ver();
	writer->writeStartElement(QLatin1String("c"));
	writer->writeDefaultNamespace(NS_CAPS);
	writer->writeAttribute(QLatin1String("hash"), QLatin1String("sha-1"));
	writer->writeAttribute(QLatin1String("ver"), ver);
	writer->writeAttribute(QLatin1String("node"), caps->node());
	writer->writeEndElement();
}

Payload::Ptr CapabilitesFactory::createPayload()
{
	return Payload::Ptr(new Capabilities(m_ver, m_node));
}


} // namespace Jreen
