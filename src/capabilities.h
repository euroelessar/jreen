/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** Jreen is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Jreen is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Jreen.  If not, see <http://www.gnu.org/licenses/>.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef JREEN_CAPABILITIES_H
#define JREEN_CAPABILITIES_H

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

#endif // JREEN_CAPABILITIES_H
