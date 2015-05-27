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

#ifndef JREEN_IQ_P_H
#define JREEN_IQ_P_H

#include "iq.h"
#include "stanza_p.h"

namespace Jreen
{
class IQPrivate : public StanzaPrivate
{
public:
	IQPrivate() 
	    : StanzaPrivate(StanzaIq), subtype(IQ::Invalid),
	      accepted(false), connection(false) {}
	IQ::Type subtype;
	mutable bool accepted;
	bool connection;
	
	static bool isConnection(const IQ &iq) { return iq.d_func()->connection; }
};

class ConnectionIQPrivate : public IQPrivate
{
};

class ConnectionIQ : public IQ
{
	Q_DECLARE_PRIVATE(ConnectionIQ)
public:
	ConnectionIQ(Type type, const JID& to, const QString& id = QString());
};
}

#endif // JREEN_IQ_P_H
