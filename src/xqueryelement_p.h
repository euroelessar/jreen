/****************************************************************************
 *  xqueryelement_p.h
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

#ifndef XQUERYELEMENT_H
#define XQUERYELEMENT_H

#include "stanza_p.h"
#include "streamfeature.h"
#include "disco.h"
#include <QLinkedList>
#include <QXmlAttributes>

namespace jreen
{

class XQueryElement;
class XQueryContainer;
typedef QSharedPointer<StreamFeature> StreamFeaturePointer;

typedef QLinkedList<XQueryElement *> XQueryElementList;
typedef QLinkedList<StreamFeaturePointer> StreamFeatureList;

class XQueryElement
{
public:
	XQueryElement(const QChar *xpath = 0, XQueryElement *parent = 0);
	XQueryElement(const QString &namespace_uri, const QString &node_name, const QXmlAttributes &attributes, XQueryElement *parent = 0);
	~XQueryElement();
	inline XQueryElement *parent() { return m_parent; }
	XQueryElement *addChild(const QChar *xpath, Disco *disco);
	void removeChild(XQueryElement *element);
	void addXQueryElement(XQueryElement *xquery);
	void parseElement(Stanza &stanza, const QDomElement &node);
	StreamFeature *findStreamFeature(const QDomElement &node);
private:
	enum XType
	{
		XNoneType = 0,
		XNodeName,
		XAttribute,
		XInteger,
		XLeftBracket
	};
	enum XOperator
	{
		XNoneOperator = 0x00,
		XEqual        = 0x01,
		XGreater      = 0x02,
		XLesser       = 0x04,
		XNotEqual     = 0x08
	};
	Q_DECLARE_FLAGS(XOperators,XOperator);
	bool check(const QDomElement &node);
	void addStanzaExtension(const StanzaExtensionPointer &stanza_extension);
	void addStreamFeature(const StreamFeaturePointer &stream_feature);
	const QChar *parseXPath(const QChar *xpath, QString &namespace_uri, QString &node_name, QXmlAttributes &attributes);
	void closeToken(XType type, QString &token, QString &namespace_uri, QString &node_name, QXmlAttributes &attributes);
	typedef QLinkedList<StanzaExtensionPointer> StanzaExtensionList;
	StanzaExtensionList m_stanza_extensions;
	StreamFeatureList m_stream_features;
	XQueryElementList m_xquery_elements;
	XQueryElement *m_parent;
	QString m_namespace_uri;
	QString m_node_name;
	QXmlAttributes m_attributes;
	friend class XQueryContainer;
};

}

#endif // XQUERYELEMENT_H
