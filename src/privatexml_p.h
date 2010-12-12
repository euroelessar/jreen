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

class PrivateXmlPrivate
{
public:
	QHash<QString, PrivateXmlTrack *> tracks;
	Client *client;
};

class PrivateXml::Query : public StanzaExtension
{
	J_EXTENSION(jreen::PrivateXml::Query,"/iq/query[@xmlns='jabber:iq:private']");
public:
	enum Type {
		Get,
		Result
	};
	Query(const QDomElement &xml)
	{
		m_type = Result;
		m_node = xml;
	}
	Query(const QString &name, const QString &xmlns)
	{
		m_type = Get;
		QDomDocument doc;
		m_node = doc.createElementNS(xmlns,name);
	}
	QDomElement xml() const { return m_node; }
	QString name() const { return m_node.nodeName(); }
	QString namespaceURI() const { return m_node.namespaceURI(); }
	Type type() const { return m_type; }
private:
	QDomElement m_node;
	Type m_type;
};

class PrivateXml::QueryFactory : public StanzaExtensionFactory<PrivateXml::Query>
{
public:
	QueryFactory();
	virtual ~QueryFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();
private:
	int m_depth;
	QDomElement m_xml;
};

enum PrivateXmlContext
{
	Request,
	Store
};

}

#endif // PRIVATEXML_P_H
