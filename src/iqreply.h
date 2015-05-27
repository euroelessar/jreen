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

#ifndef JREEN_IQREPLY_H
#define JREEN_IQREPLY_H

#include "jreen.h"

namespace Jreen
{
class Client;
class ClientPrivate;
class IQ;
class IQReplyPrivate;

class JREEN_EXPORT IQReply : public QObject
{
    Q_OBJECT
	Q_DECLARE_PRIVATE(IQReply)
public:
	~IQReply();

	QVariant data() const;
	void setData(const QVariant &data);
	
signals:
	void received(const Jreen::IQ &iq);
	
protected:
    explicit IQReply(Client *client);
	
	friend class Jreen::ClientPrivate;
	QScopedPointer<IQReplyPrivate> d_ptr;
};
}

#endif // JREEN_IQREPLY_H
