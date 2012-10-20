/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "jingletransport.h"
#include "jinglesession.h"

namespace Jreen
{

class JingleTransportPrivate
{
public:
	JingleTransportInfo::Ptr localInfo;
	JingleTransport::State state;
};

JingleTransport::JingleTransport(JingleContent *content)
    : QObject(content), d_ptr(new JingleTransportPrivate)
{
	Q_D(JingleTransport);
	d->state = Gathering;
}

JingleTransport::~JingleTransport()
{
}

JingleTransport::State JingleTransport::state() const
{
	return d_func()->state;
}

JingleTransportInfo::Ptr JingleTransport::localInfo() const
{
	return d_func()->localInfo;
}

void JingleTransport::setState(JingleTransport::State state)
{
	d_func()->state = state;
	emit stateChanged(state);
}

void JingleTransport::setLocalInfo(const JingleTransportInfo::Ptr &info)
{
	d_func()->localInfo = info;
	emit localInfoReady(info);
}

}
