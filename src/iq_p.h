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

#ifndef IQ_P_H
#define IQ_P_H

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

#endif // IQ_P_H
