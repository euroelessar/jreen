/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef CAPABILITIES_H
#define CAPABILITIES_H

#include "stanzaextension.h"

namespace Jreen
{

class CapabilitiesPrivate;
class JREEN_EXPORT Capabilities : public Payload
{
	Q_DECLARE_PRIVATE(Capabilities)
	J_PAYLOAD(Jreen::Capabilities)
public:
	Capabilities(const QString &ver, const QString &node = QString());
	~Capabilities();
	
	void setNode(const QString &node);
	QString node() const;
	QString ver() const;
	QString calculatedNode() const;
private:
	QScopedPointer<CapabilitiesPrivate> d_ptr;
};

}

#endif // CAPABILITIES_H
