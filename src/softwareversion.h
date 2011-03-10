/****************************************************************************
 *  softwareversion.h
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

#ifndef SOFTWAREVERSION_H
#define SOFTWAREVERSION_H

#include "stanzaextension.h"

namespace Jreen
{

/*
 * Implementation of XEP-0092
 * http://xmpp.org/extensions/xep-0092.html
 */

class JREEN_EXPORT SoftwareVersion : public StanzaExtension
{
	J_EXTENSION(Jreen::SoftwareVersion,"/iq/query[@xmlns='jabber:iq:version']")
public:
	SoftwareVersion() {}
	SoftwareVersion(const QString &name, const QString &version, const QString &os = QString());
	inline const QString &name() const { return m_name; }
	inline const QString &version() const { return m_version; }
	inline const QString &os() const { return m_os; }
private:
	QString m_name;
	QString m_version;
	QString m_os;
};

}

#endif // SOFTWAREVERSION_H
