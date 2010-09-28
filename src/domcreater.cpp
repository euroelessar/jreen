/****************************************************************************
 *  domcreater.cpp
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

#include "domcreater_p.h"

namespace jreen
{

QDomDocument DomCreater::m_document = QDomDocument();

QDomElement createElement(QDomDocument *doc, const QString &name, const QString &value)
{
	QDomElement node = doc->createElement(name);
	if(!value.isEmpty())
		node.appendChild(doc->createTextNode(value));
	return node;
}

QDomElement createElement(QDomElement parent, const QString &name, const QString &value)
{
	QDomDocument doc = parent.ownerDocument();
	QDomElement node = doc.createElement(name);
	if(!value.isEmpty())
		node.appendChild(doc.createTextNode(value));
	parent.appendChild(node);
	return node;
}

}
