/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "chatstate.h"

namespace Jreen {

class ChatStatePrivate
{
public:
	ChatState::State state;
};

ChatState::ChatState(Jreen::ChatState::State state)
    : d_ptr(new ChatStatePrivate)
{
	d_func()->state = state;
}

ChatState::~ChatState()
{
}

ChatState::State ChatState::state() const
{
	return d_func()->state;
}

}
 // namespace Jreen
