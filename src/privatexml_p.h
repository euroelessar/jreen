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

namespace Jreen
{

class PrivateXmlTrack : public QObject
{
	Q_OBJECT
	friend class PrivateXml;
	inline PrivateXmlTrack(QObject *handler, const char *member)
	{
		connect(this, SIGNAL(resultReady(Jreen::Payload::Ptr,Jreen::PrivateXml::Result,Jreen::Error::Ptr)),
				handler, member);
	}
signals:
	void resultReady(const Jreen::Payload::Ptr &,Jreen::PrivateXml::Result,const Jreen::Error::Ptr &);
};

class PrivateXmlPrivate
{
public:
	QHash<QString, PrivateXmlTrack *> tracks;
	Client *client;
};

class PrivateXmlQuery : public Payload
{
	J_PAYLOAD(Jreen::PrivateXmlQuery);
public:
	enum Type {
		Get,
		Result
	};
	PrivateXmlQuery(const Payload::Ptr &xml)
	{
		m_type = Result;
		m_node = xml;
	}
	PrivateXmlQuery(const QString &name, const QString &xmlns)
	{
		m_type = Get;
		m_name = name;
		m_namespaceUri = xmlns;
	}
	Payload::Ptr xml() const { return m_node; }
	QString name() const { return m_name; }
	QString namespaceURI() const { return m_namespaceUri; }
	Type type() const { return m_type; }
private:
	Payload::Ptr m_node;
	QString m_name;
	QString m_namespaceUri;
	Type m_type;
};

class PrivateXmlQueryFactory : public PayloadFactory<PrivateXmlQuery>
{
public:
	PrivateXmlQueryFactory(Client *client);
	virtual ~PrivateXmlQueryFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(Payload *extension, QXmlStreamWriter *writer);
	Payload::Ptr createPayload();
private:
	int m_depth;
	AbstractPayloadFactory *m_factory;
	ClientPrivate *m_client;
	Payload::Ptr m_node;
};

enum PrivateXmlContext
{
	Request,
	Store
};

}

#endif // PRIVATEXML_P_H
