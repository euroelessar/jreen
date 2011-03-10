/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef PUBSUBEVENTFACTORY_H
#define PUBSUBEVENTFACTORY_H

#include "pubsubevent.h"
#include <QMap>

namespace Jreen
{
namespace PubSub
{
class EventFactory : public StanzaExtensionFactory<Event>
{
public:
	EventFactory(QList<AbstractStanzaExtensionFactory*> &factories);
	~EventFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
private:
	AbstractStanzaExtensionFactory *findFactory(const QStringRef &node);
	AbstractStanzaExtensionFactory *findFactory(int type);

	enum State { AtNowhere, AtItems, AtItem, AtEntity } m_state;
	QScopedPointer<Event> m_event;
	AbstractStanzaExtensionFactory *m_factory;
	QList<AbstractStanzaExtensionFactory*> &m_factories;
	int m_depth;
};
}
}

#endif // PUBSUBEVENTFACTORY_H
