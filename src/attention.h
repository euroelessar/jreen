/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
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
#ifndef ATTENTION_H
#define ATTENTION_H
#include "stanzaextension.h"

namespace Jreen {

class AttentionPrivate;

//XEP-0224 Attention
//http://xmpp.org/extensions/xep-0224.html
class JREEN_EXPORT Attention : public Payload
{
	Q_DECLARE_PRIVATE(Attention)
	J_PAYLOAD(Jreen::Attention)
public:
	Attention();
	~Attention();
private:
	QScopedPointer<AttentionPrivate> d_ptr;
};

} // namespace Jreen

#endif // ATTENTION_H
