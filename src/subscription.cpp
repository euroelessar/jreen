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

#include "subscription.h"
#include "stanza_p.h"
#include "langmap.h"

namespace Jreen
{

//J_STRING(presence)
//J_STRING(status)

//static const QStringList s10n_types = QStringList() << QLatin1String("subscribe") << QLatin1String("subscribed")
//									  << QLatin1String("unsubscribe") << QLatin1String("unsubscribed");

//struct SubscriptionPrivate : public StanzaPrivate
//{
//	SubscriptionPrivate() : StanzaPrivate(StanzaSubscription) {}
//	Subscription::Type subtype;
//	LangMap status;
//};

//Subscription::Subscription(const QDomElement &node) : Stanza(node, new SubscriptionPrivate)
//{
//	Q_D(Subscription);
//	int type = s10n_types.indexOf(node.attribute(ConstString::type));
//	d->subtype = type < 0 ? Invalid : static_cast<Type>(type);
//	forelements(const QDomElement &elem, node)
//		if( elem.nodeName() == status_str)
//		{
//			QString lang = elem.attribute(ConstString::lang);
//			d->status[lang] = elem.text();
//		}
//}

//Subscription::Subscription(Type type, const JID& to, const QString &status, const QString &xmllang)
//	: Stanza(*new SubscriptionPrivate)
//{
//	Q_D(Subscription);
//	d->subtype = type;
//	d->to = to;
//	d->status[xmllang] = status;
//}

//Subscription::Type Subscription::subtype() const
//{
//	Q_D(const Subscription);
//	return d->subtype;
//}

//const QString &Subscription::status(const QString &lang) const
//{
//	Q_D(const Subscription);
//	return d->status.value(lang);
//}

//void Subscription::writeXml(QXmlStreamWriter *writer) const
//{
//	Q_D(const Subscription);
//	writer->writeStartElement(presence_str);
//	d->setAttributes(writer);
//	if (d->subtype != Invalid) {
//		writer->writeAttribute(ConstString::type, s10n_types.at(d->subtype));
//		d->addExtensions(writer);
//	}
//	writer->writeEndElement();
//}

}
