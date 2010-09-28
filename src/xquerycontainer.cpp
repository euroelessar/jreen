/****************************************************************************
 *  xquerycontainer.cpp
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

#include "xquerycontainer_p.h"

J_BEGIN_NAMESPACE

XQueryContainer::XQueryContainer()
{
}

XQueryContainer::~XQueryContainer()
{
	qDeleteAll( m_xquery_elements );
}

void XQueryContainer::registerStanzaExtension( StanzaExtension *extension, Disco *disco )
{
	if( !extension )
		return;
	StanzaExtensionPointer stanza_extension( extension );
	if( m_all_stanza_extensions.contains( stanza_extension->extensionType() ) )
	{
		QList<XQueryElement *> elements = m_all_stanza_extensions.values( stanza_extension->extensionType() );
		foreach( XQueryElement *element, elements )
		{
			if( element->parent() )
				element->parent()->removeChild( element );
		}
		m_all_stanza_extensions.remove( stanza_extension->extensionType() );
	}
	QStringList xpaths = stanza_extension->xPath().split( '|' );
	foreach( const QString &xpath, xpaths )
	{
		if( xpath.startsWith( QLatin1String("//") ) )
			continue;
		XQueryElement *xquery = addChild( xpath.constData(), disco );
		xquery->addStanzaExtension( stanza_extension );
		m_all_stanza_extensions.insert( stanza_extension->extensionType(), xquery );
	}
}

void XQueryContainer::registerStreamFeature( StreamFeature *feature )
{
	if( !feature )
		return;
	StreamFeaturePointer stream_feature( feature );
	QStringList xpaths = stream_feature->xPath().split( '|' );
	foreach( const QString &xpath, xpaths )
	{
		if( xpath.startsWith( QLatin1String("//") ) )
			continue;
		XQueryElement *xquery = addChild( xpath.constData(), 0 );
		xquery->addStreamFeature( stream_feature );
		m_all_stream_features.append( xquery );
	}
}

void XQueryContainer::parseElement( Stanza &stanza, const QDomElement &node )
{
	foreach( XQueryElement *element, m_xquery_elements )
		element->parseElement( stanza, node );
}

StreamFeature *XQueryContainer::findStreamFeature( const QDomElement &node )
{
	foreach( XQueryElement *element, m_xquery_elements )
	{
		StreamFeature *stream_feature = element->findStreamFeature( node );
		if( stream_feature )
			return stream_feature;
	}
	return 0;
}

void XQueryContainer::resetFeatures()
{
	foreach( const StreamFeaturePointer &stream_feature, m_stream_features )
		stream_feature->reset();
}

J_END_NAMESPACE
