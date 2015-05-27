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

#ifndef JREEN_STREAMPROCESSOR_H
#define JREEN_STREAMPROCESSOR_H

#include "jreen.h"
#include <QObject>

namespace Jreen
{
class StreamInfo;
class JID;
class JREEN_EXPORT StreamProcessor
{
public:
    StreamProcessor();
	virtual ~StreamProcessor();

	virtual void authorized() = 0;
	virtual QString sessionID() const = 0;
	virtual void setJID(const JID &jid) = 0;
	virtual void setStreamParser(XmlStreamParser *parser) = 0;
	virtual void restartStream() = 0;
};
}

Q_DECLARE_INTERFACE(Jreen::StreamProcessor, "org.qutim.Jreen.StreamProcessor")

#endif // JREEN_STREAMPROCESSOR_H
