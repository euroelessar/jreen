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

#ifndef NICKNAME_H
#define NICKNAME_H

#include "stanzaextension.h"

namespace Jreen
{

class NicknamePrivate;
class JREEN_EXPORT Nickname : public Payload
{
	Q_DECLARE_PRIVATE(Nickname)
	J_PAYLOAD(Jreen::Nickname)
public:
	Nickname(const QString &nick);
	~Nickname();
	
	QString nick() const;
private:
	QScopedPointer<NicknamePrivate> d_ptr;
};

}

#endif // NICKNAME_H
