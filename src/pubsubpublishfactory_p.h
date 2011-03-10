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

#ifndef PUBSUBPUBLISHFACTORY_P_H
#define PUBSUBPUBLISHFACTORY_P_H

#include "pubsubmanager_p.h"
#include "dataformfactory_p.h"

namespace Jreen
{
namespace PubSub
{
class PublishFactory : public StanzaExtensionFactory<Publish>
{
public:
	PublishFactory(QList<AbstractStanzaExtensionFactory*> &factories);
	~PublishFactory();
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

	enum State { AtNowhere, AtPublish, AtItem, AtEntity } m_state;
	QScopedPointer<Publish> m_publish;
	AbstractStanzaExtensionFactory *m_factory;
	QList<AbstractStanzaExtensionFactory*> &m_factories;
	int m_depth;
	DataFormFactory m_form;
};
}
}

#endif // PUBSUBPUBLISHFACTORY_P_H
