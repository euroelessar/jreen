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

namespace jreen
{

/*
 * Implementation of XEP-0092
 */

class JREEN_EXPORT SoftwareVersion : public StanzaExtension
{
	J_EXTENSION(jreen::SoftwareVersion,"/iq/query[@xmlns='jabber:iq:version']")
public:
	SoftwareVersion(const QString &name, const QString &version, const QString &os = QString());
	inline const QString &name() { return m_name; }
	inline const QString &version() { return m_version; }
	inline const QString &os() { return m_os; }
private:
	QString m_name;
	QString m_version;
	QString m_os;
};

}

#endif // SOFTWAREVERSION_H
