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

namespace Jreen
{

class CapabilitiesPrivate;
class JREEN_EXPORT Capabilities : public StanzaExtension
{
	Q_DECLARE_PRIVATE(Capabilities)
	J_EXTENSION(Jreen::Capabilities,"/presence/c[@xmlns='http://jabber.org/protocol/caps']")
public:
	Capabilities(const QString &ver, const QString &node = QString());
	~Capabilities();
	
	void setNode(const QString &node);
	QString node() const;
	QString ver() const;
private:
	QScopedPointer<CapabilitiesPrivate> d_ptr;
};

}

J_DECLARE_EXTENSION(Jreen::Capabilities)

#endif // CAPABILITIES_H
