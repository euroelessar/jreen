/****************************************************************************
 *  xqueryelement.cpp
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

#include "xqueryelement_p.h"

//#define XQUERY_DEBUG

namespace jreen
{

XQueryElement::XQueryElement(const QChar *xpath, XQueryElement *parent) : m_parent(parent)
{
	if(!xpath)
		return;
	parseXPath(xpath, m_namespace_uri, m_node_name, m_attributes);
}

XQueryElement::XQueryElement(const QString &namespace_uri, const QString &node_name,
							  const QXmlAttributes &attributes, XQueryElement *parent)
		: m_parent(parent), m_namespace_uri(namespace_uri), m_node_name(node_name), m_attributes(attributes)
{
}

XQueryElement::~XQueryElement()
{
	m_stanza_extensions.clear();
	m_stream_features.clear();
	qDeleteAll(m_xquery_elements);
	m_xquery_elements.clear();
}

void XQueryElement::removeChild(XQueryElement *element)
{
	delete element;
	m_xquery_elements.removeAll(element);
	if(m_parent && m_xquery_elements.isEmpty() && m_stanza_extensions.isEmpty())
		m_parent->removeChild(this);
}

XQueryElement *XQueryElement::addChild(const QChar *xpath, Disco *disco)
{
	if((*xpath).isNull())
		return this;
	QString node_name, namespace_uri;
	QXmlAttributes attributes;
	const QChar *cur = XQueryElement::parseXPath(xpath, namespace_uri, node_name, attributes);
	if(disco && !namespace_uri.isEmpty())
		disco->features() << namespace_uri;
	XQueryElementList::iterator i = m_xquery_elements.begin();
	XQueryElement *element = 0;
	for(; i != m_xquery_elements.end(); i++)
	{
		// TODO: Add compare of attributes
		if(node_name == (*i)->m_node_name && namespace_uri == (*i)->m_namespace_uri && attributes.count() == (*i)->m_attributes.count())
		{
			element = *i;
			break;
		}
	}
	if(element == 0)
	{
		element = new XQueryElement(namespace_uri, node_name, attributes, this);
		m_xquery_elements.append(element);
	}
	return element->addChild(cur, disco);
}

const QChar *XQueryElement::parseXPath(const QChar *c, QString &namespace_uri, QString &node_name, QXmlAttributes &attributes)
{
	if((*c).isNull())
		return c;
	QString token;
	XType type = XNodeName;
	bool first_quote = false;
	bool second_quote = false;
#ifdef XQUERY_DEBUG
	qDebug("begin parse");
#endif
	while(!(*++c).isNull() && (((*c) != '/' && (*c) != '|') || first_quote || second_quote))
	{
#ifdef XQUERY_DEBUG
		qDebug("new char: ['%lc']", *(const ushort*)c);
#endif
		if((first_quote && (*c).unicode() != L'\'')
			|| (second_quote && (*c).unicode() != L'"'))
		{
			token.append(*c);
			continue;
		}
		switch((*c).unicode())
		{
		case L'[':
			closeToken(type, token, namespace_uri, node_name, attributes);
			type = XInteger;
			break;
		case L']':
			closeToken(type, token, namespace_uri, node_name, attributes);
			type = XNoneType;
			break;
		case L'@':
			type = XAttribute;
			break;
		case L'\'':
			first_quote = !first_quote;
			token.append(*c);
			break;
		case L'"':
			second_quote = !second_quote;
			token.append(*c);
			break;
		default:
			if(type == XInteger && !(*c).isDigit())
				type = XLeftBracket;
			token.append(*c);
		}
	}
	closeToken(type, token, namespace_uri, node_name, attributes);
#ifdef XQUERY_DEBUG
	qDebug("end parse: <%s xmlns='%s'/>", qPrintable(node_name), qPrintable(namespace_uri));
#endif
	return c;
}

void XQueryElement::closeToken(XType type, QString &token, QString &namespace_uri, QString &node_name, QXmlAttributes &attributes)
{
#ifdef XQUERY_DEBUG
	qDebug("close token: %d %s", (int)type, qPrintable(token));
#endif
	switch(type)
	{
	case XNodeName:
		node_name = token == QLatin1String("*") ? QString() : token;
		token.clear();
		break;
	case XAttribute:{
		bool first_quote = false;
		bool second_quote = false;
		QString qname;
		QString lname;
		QString value;
		bool have_colon = false;
		XOperators opers;
		const QChar *c = token.constData();
		while(!(*c).isNull())
		{
#ifdef XQUERY_DEBUG
			qDebug("new char at token: ['%lc']", *(const ushort*)c);
#endif
			if((first_quote && (*c).unicode() != L'\'')
				|| (second_quote && (*c).unicode() != L'"'))
			{
				if(!opers)
					qname.append(*c);
				else
					value.append(*c);
				c++;
				continue;
			}
			switch((*c).unicode())
			{
			case L'>':
				opers |= XGreater;
				break;
			case L'=':
				opers |= XEqual;
				break;
			case L'<':
				opers |= XLesser;
				break;
			case L'!':
				opers |= XNotEqual;
				break;
			case L'\'':
				first_quote = !first_quote;
				break;
			case L'"':
				second_quote = !second_quote;
				break;
			case L':':
				if(!have_colon && !opers)
					have_colon = true;
			default:
				if(!opers)
				{
					if(have_colon)
						lname.append(*c);
					qname.append(*c);
				}
				else
					value.append(*c);
			}
			c++;
		}
		if(lname.isEmpty())
			lname = qname;
//		xml:lang='en'
//		xml:lang http://www.w3.org/XML/1998/namespace lang en
//		qName    uri                                  ln   value
#ifdef XQUERY_DEBUG
		qDebug("attribute parsed: %s='%s'", qPrintable(qname), qPrintable(value));
#endif
		if(qname == QLatin1String("xmlns") || qname.startsWith(QLatin1String("xmlns:")))
			namespace_uri = value;
		else
			attributes.append(qname, QLatin1String(""), lname, value);
		token.clear();
		break;}
	default:
		token.clear();
		break;
	}
}

void XQueryElement::addXQueryElement(XQueryElement *xquery)
{
	if(xquery)
		m_xquery_elements.append(xquery);
}

void XQueryElement::addStanzaExtension(const StanzaExtensionPointer &stanza_extension)
{
	m_stanza_extensions.append(stanza_extension);
}
void XQueryElement::addStreamFeature(const StreamFeaturePointer &stream_feature)
{
	m_stream_features.append(stream_feature);
}

bool XQueryElement::check(const QDomElement &node)
{
#ifdef XQUERY_DEBUG
	qDebug("name: %s %s\nuri:  %s %s", m_node_name.isEmpty() ? "*" : qPrintable(m_node_name), qPrintable(node.nodeName()),
			m_namespace_uri.isEmpty() ? "*" : qPrintable(m_namespace_uri), qPrintable(node.namespaceURI()));
#endif
	if(!m_node_name.isEmpty() && node.nodeName() != m_node_name)
		return false;
	if(!m_namespace_uri.isEmpty() && node.namespaceURI() != m_namespace_uri)
		return false;
	for(int k = 0; k < m_attributes.count(); k++)
	{
		if(node.attribute(m_attributes.qName(k)) != m_attributes.value(k))
			return false;
	}
	return true;
}

StreamFeature *XQueryElement::findStreamFeature(const QDomElement &node)
{
	if(!check(node))
		return 0;
	if(m_stream_features.size())
		return m_stream_features.first().data();
	forelements(const QDomElement &element, node)
	{
		XQueryElementList::iterator j = m_xquery_elements.begin();
		for(; j != m_xquery_elements.end(); j++)
		{
			StreamFeature *stream_feature = (*j)->findStreamFeature(element);
			if(stream_feature)
				return stream_feature;
		}
	}
	return 0;
}

void XQueryElement::parseElement(Stanza &stanza, const QDomElement &node)
{
	if(!check(node))
		return;
	StanzaExtensionList::iterator i = m_stanza_extensions.begin();
	for(; i != m_stanza_extensions.end(); i++)
	{
		stanza.addExtension((*i)->fromNode(node));
	}
	forelements(const QDomElement &element, node)
	{
		XQueryElementList::iterator j = m_xquery_elements.begin();
		for(; j != m_xquery_elements.end(); j++)
		{
			(*j)->parseElement(stanza, element);
		}
	}
}

}
