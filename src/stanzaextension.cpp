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
#include "stanzaextension.h"
#include <QHash>

namespace Jreen
{
typedef QHash<QByteArray, int> ByteArrayHash;
Q_GLOBAL_STATIC(ByteArrayHash, seClassHash)
Q_GLOBAL_STATIC(QVector<QByteArray>, seClassVector)

Payload::Payload()
{
}

Payload::~Payload()
{
}

int Payload::registerPayloadType(const char *type)
{
	QByteArray t = type;
	int id = seClassHash()->value(t, seClassHash()->size());
	if (id == seClassHash()->size()) {
		ByteArrayHash::Iterator it = seClassHash()->insert(t, id);
		seClassVector()->append(it.key());
	}
	return id;
}

const char *Payload::payloadName(int type)
{
	return seClassVector()->value(type).constData();
}

const char *Payload::payloadName() const
{
	return payloadName(payloadType());
}

AbstractPayloadFactory::AbstractPayloadFactory()
{
}

AbstractPayloadFactory::~AbstractPayloadFactory()
{
}
}
