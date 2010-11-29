/****************************************************************************
 *  capabilities.h
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

#ifndef CAPABILITIES_H
#define CAPABILITIES_H

#include "stanzaextension.h"

namespace jreen
{

class Disco;

class JREEN_EXPORT Capabilities : public StanzaExtension
{
	J_EXTENSION(jreen::Capabilities,"/presence/c[@xmlns='http://jabber.org/protocol/caps']")
public:
	Capabilities(const QString &ver);
//	Capabilities(const QDomElement &node = QDomElement());
//	void writeXml(QXmlStreamWriter *writer) const;
	inline void setNode(const QString &node) { m_node = node; }
	inline const QString &node() const { return m_node; }
	QString ver() const { return m_ver; };
private:
	QString m_ver;
	QString m_node;
};

}

J_DECLARE_EXTENSION(jreen::Capabilities)

#endif // CAPABILITIES_H
