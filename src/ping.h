/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef PING_H
#define PING_H
#include "stanzaextension.h"
#include "jid.h"

namespace Jreen {

//XEP-0199
//http://xmpp.org/extensions/xep-0199.html
class PingPrivate;
class JREEN_EXPORT Ping : public Payload
{
	Q_DECLARE_PRIVATE(Ping)
	J_PAYLOAD(Jreen::Ping)
public:
	Ping();
	~Ping();
	
private:
	QScopedPointer<PingPrivate> d_ptr;
};

} // namespace Jreen

#endif // PING_H
