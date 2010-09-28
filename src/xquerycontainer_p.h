/****************************************************************************
 *  xquerycontainer_p.h
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

#ifndef XQUERYCONTAINER_H
#define XQUERYCONTAINER_H

#include "xqueryelement_p.h"
#include <QMap>

namespace jreen
{

class XQueryContainer : public XQueryElement
{
	QMultiMap<int, XQueryElement *> m_all_stanza_extensions;
	QList<XQueryElement *> m_all_stream_features;
public:
	XQueryContainer();
	~XQueryContainer();
	void registerStanzaExtension( StanzaExtension *stanza_extension, Disco *disco );
	void registerStreamFeature( StreamFeature *stream_feature );
	void parseElement( Stanza &stanza, const QDomElement &node );
	StreamFeature *findStreamFeature( const QDomElement &node );
	void resetFeatures();
};

}

#endif // XQUERYCONTAINER_H
