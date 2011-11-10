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

class SoftwareVersionPrivate;
class JREEN_EXPORT SoftwareVersion : public Payload
{
	Q_DECLARE_PRIVATE(SoftwareVersion)
	J_PAYLOAD(Jreen::SoftwareVersion)
public:
	SoftwareVersion();
	SoftwareVersion(const QString &name, const QString &version, const QString &os = QString());
	~SoftwareVersion();
	
	QString name() const;
	QString version() const;
	QString os() const;
private:
	QScopedPointer<SoftwareVersionPrivate> d_ptr;
	QString m_name;
	QString m_version;
	QString m_os;
};

}

#endif // SOFTWAREVERSION_H
