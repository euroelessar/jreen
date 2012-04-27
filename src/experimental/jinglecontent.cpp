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

#include "jinglecontent_p.h"
#include "jingletransport.h"
#include "jinglesession_p.h"
#include "../logger.h"

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

void JingleContentPrivate::_q_localInfoReady(const Jreen::JingleTransportInfo::Ptr &)
{
	if (needTransports > 0) {
		needTransports--;
		if (needTransports == 0) {
			JingleSessionPrivate *sessionD = JingleSessionPrivate::get(session);
			sessionD->onTransportsReady(q_func(), transports);
		}
	} else {
		canAccept = 1;
		transportInfos.clear();
		transport = qobject_cast<JingleTransport*>(q_func()->sender());
		Q_ASSERT(transport);
		transports << transport;
		accept();
	}
}

void JingleContentPrivate::_q_tryStateChanged(Jreen::JingleTransport::State state)
{
	if (state == JingleTransport::Failed) {
		JingleTransport *transport = qobject_cast<JingleTransport*>(q_func()->sender());
		Q_ASSERT(transport);
		delete transport;
		tryNextTransport();
	}
}

void JingleContentPrivate::setTransport(JingleTransport *trueTransport)
{
	transport = trueTransport;
	Logger::debug() << Q_FUNC_INFO << transport;
	QObject::connect(transport, SIGNAL(received(int,QByteArray)),
	                 q_func(), SLOT(_q_received(int,QByteArray)));
	QObject::connect(transport, SIGNAL(stateChanged(Jreen::JingleTransport::State)),
	                 q_func(), SLOT(_q_stateChanged(Jreen::JingleTransport::State)));
}

void JingleContentPrivate::initiateTransports()
{
	JingleSessionPrivate *sessionD = JingleSessionPrivate::get(session);
	JingleManagerPrivate *manager = JingleManagerPrivate::get(sessionD->client->jingleManager());
	foreach (AbstractJingleTransportFactory *factory, manager->transports) {
		JingleTransport *transport = factory->createObject(q_func());
		if (transport->localInfo().isNull()) {
			QObject::connect(transport, SIGNAL(localInfoReady(Jreen::JingleTransportInfo::Ptr)),
			                 q_func(), SLOT(_q_localInfoReady(Jreen::JingleTransportInfo::Ptr)));
			needTransports++;
		}
		transports << transport;
	}
}

void JingleContentPrivate::accept()
{
	if (needAccept || !canAccept)
		return;
	JingleSessionPrivate *sessionD = JingleSessionPrivate::get(session);
	sessionD->onTransportsReady(q_func(), transports);
}

void JingleContentPrivate::decline()
{
	IQReply *reply = Jingle::send(session, Jingle::ContentReject, q_func());
	Q_UNUSED(reply);
}

void JingleContentPrivate::tryNextTransport()
{
	JingleSessionPrivate *sessionD = JingleSessionPrivate::get(session);
	JingleManagerPrivate *manager = JingleManagerPrivate::get(sessionD->client->jingleManager());
	JingleTransport *transport = 0;
	JingleTransportInfo::Ptr info;
	while (!transport && !transportInfos.isEmpty()) {
		info = transportInfos.takeFirst();
		transport = manager->transport(info, q_func());
	}
	if (!transport) {
		q_func()->decline();
		return;
	}
	transport->setRemoteInfo(info, false);
	QObject::connect(transport, SIGNAL(localInfoReady(Jreen::JingleTransportInfo::Ptr)),
	                 q_func(), SLOT(_q_localInfoReady(Jreen::JingleTransportInfo::Ptr)));
	QObject::connect(transport, SIGNAL(stateChanged(Jreen::JingleTransport::State)),
	                 q_func(), SLOT(_q_stateChanged(Jreen::JingleTransport::State)));
}

void JingleContentPrivate::initiateTransports(const QList<JingleTransportInfo::Ptr> &transportInfosOther)
{
	transportInfos = transportInfosOther;
	tryNextTransport();
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

bool JingleContent::isAcceptable() const
{
	return d_func()->needAccept;
}

void JingleContent::accept()
{
	Q_D(JingleContent);
	d->needAccept = false;
	d->accept();
}

void JingleContent::decline()
{
	Q_D(JingleContent);
	if (d->needAccept) {
		d->needAccept = false;
		IQReply *reply = Jingle::send(d->session, Jingle::ContentReject, this);
		Q_UNUSED(reply);
	}
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

#include "moc_jinglecontent.cpp"
