/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "jinglecontent_p.h"
#include "jingletransport.h"
#include "jinglesession.h"
#include <QDebug>

namespace Jreen
{

void JingleContentPrivate::_q_received(int component, const QByteArray &data)
{
	q_func()->receive(component, data);
}

void JingleContentPrivate::_q_stateChanged(Jreen::JingleTransport::State newState)
{
	state = static_cast<JingleContent::State>(newState);
	emit q_func()->stateChanged(state);
}

void JingleContentPrivate::setTransport(JingleTransport *trueTransport)
{
	transport = trueTransport;
	qDebug() << Q_FUNC_INFO << transport;
	QObject::connect(transport, SIGNAL(received(int,QByteArray)),
	                 q_func(), SLOT(_q_received(int,QByteArray)));
	QObject::connect(transport, SIGNAL(stateChanged(Jreen::JingleTransport::State)),
	                 q_func(), SLOT(_q_stateChanged(Jreen::JingleTransport::State)));
}

JingleContent::JingleContent(JingleSession *session)
    : QObject(session), d_ptr(new JingleContentPrivate(this))
{
	Q_D(JingleContent);
	d->session = session;
}

JingleContent::JingleContent(JingleSession *session, JingleContentPrivate &p)
    : QObject(session), d_ptr(&p)
{
	Q_D(JingleContent);
	d->session = session;
}

JingleContent::~JingleContent()
{
}

JingleSession *JingleContent::session() const
{
	return d_func()->session;
}

JingleContent::State JingleContent::state() const
{
	return d_func()->state;
}

int JingleContent::componentCount() const
{
	return d_func()->componentCount;
}

void JingleContent::setComponentCount(int count)
{
	d_func()->componentCount = count;
}

void JingleContent::send(int component, const QByteArray &data)
{
	Q_D(JingleContent);
	if (d->transport)
		d->transport->send(component, data);
}

void JingleContent::send(int component, const char *data, int size)
{
	send(component, QByteArray(data, size));
}

}

#include "jinglecontent.moc"
