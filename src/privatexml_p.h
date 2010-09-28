/****************************************************************************
 *  privatexml_p.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef PRIVATEXML_P_H
#define PRIVATEXML_P_H

#include "privatexml.h"
#include "domcreater_p.h"
#include "jstrings.h"
#include "iq.h"
#include "client.h"
#include "error.h"
#include <QHash>

namespace jreen
{

class PrivateXmlTrack : public QObject
{
	Q_OBJECT
	friend class PrivateXml;
	inline PrivateXmlTrack(QObject *handler, const char *member)
	{
		connect(this, SIGNAL(newResult(QDomElement,PrivateXml::Result,QSharedPointer<Error>)), handler, member);
	}
signals:
	void newResult(const QDomElement &node, PrivateXml::Result result, QSharedPointer<Error> error);
};

struct PrivateXmlPrivate
{
	QHash<QString, PrivateXmlTrack *> tracks;
	Client *client;
};

class PrivateXml::Query : public StanzaExtension
{
	J_EXTENSION(Query,"/iq/query[@xmlns='jabber:iq:private']");
public:
	Query(const QDomElement &node = QDomElement())
	{
		if(node.nodeName() == ConstString::query && node.namespaceURI() == ConstString::xmlns_private_xml)
			m_node = node.firstChildElement();
		else
			m_node = node;
	}
	Query(const QString &name, const QString &xmlns)
	{
		m_node = DomCreater::instance().createElement(name);
		m_node.setAttribute(ConstString::xmlns, xmlns);
	}
	QDomElement node(QDomDocument *document) const
	{
		QDomElement node = createElement(document, ConstString::query);
		node.setAttribute(ConstString::xmlns, ConstString::xmlns_private_xml);
		node.appendChild(m_node);
		return node;
	}
	inline const QDomElement &xml() { return m_node; }
private:
	QDomElement m_node;
};

enum PrivateXmlContext
{
	Request,
	Store
};

}

#endif // PRIVATEXML_P_H
