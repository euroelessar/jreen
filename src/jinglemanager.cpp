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

#include "jinglemanager_p.h"
#include "client.h"
#include "jinglesession_p.h"
#include "jingletransportice_p.h"
#include "jingleaudiocontentfactory_p.h"

namespace Jreen
{

JingleContent *JingleManagerPrivate::content(const QString &name, JingleSession *session)
{
	for (int i = 0; i < descriptions.size(); ++i) {
		if (descriptions.at(i)->media() == name)
			return descriptions.at(i)->createObject(session);
	}
	return 0;
}

void JingleManagerPrivate::_q_iqReceived(const Jreen::IQ &iq)
{
	Jingle::Ptr jingle = iq.payload<Jingle>();
	if (!jingle)
		return;
	qDebug() << Q_FUNC_INFO;
	iq.accept();
	JingleSession *session = sessions.value(jingle->sid);
	if (session) {
		JingleSessionPrivate::get(session)->handle(jingle);
	} else if (jingle->action == Jingle::SessionInitiate) {
		new JingleSession(jingle, client);
	} else {
		IQ error(IQ::Error, iq.from(), iq.id());
		Jingle::Ptr result = Jingle::Ptr::create();
		result->sid = jingle->sid;
		result->initiator = jingle->initiator;
		result->action = Jingle::SessionTerminate;
		// TODO: Add reason
		client->send(error);
		return;
	}
	IQ reply(IQ::Result, iq.from());
	client->send(reply);
}

JingleManager::JingleManager(Client *client)
    : QObject(client), d_ptr(new JingleManagerPrivate)
{
	Q_D(JingleManager);
	d->client = client;
	d->transports << new JingleIce::TransportFactory;
	d->descriptions << new JingleAudioContentFactory;
	client->registerPayload(new JingleFactory(client));
	foreach (AbstractPayloadFactory *factory, d->transports)
		client->registerPayload(factory);
	foreach (AbstractPayloadFactory *factory, d->descriptions)
		client->registerPayload(factory);
	connect(d->client, SIGNAL(iqReceived(Jreen::IQ)),
	        SLOT(_q_iqReceived(Jreen::IQ)));
}

JingleManager::~JingleManager()
{
}

static inline bool set_contains_list(const QSet<QString> &features, const QStringList &list)
{
	bool ok = true;
	for (int i = 0; ok && i < list.size(); ++i)
		ok &= features.contains(list.at(i));
	return ok;
}

bool JingleManager::checkSupport(const QSet<QString> &features)
{
	Q_D(JingleManager);
	bool ok = JingleFactory::checkSupport(features);
	if (!ok) return false;
	ok = false;
	for (int i = 0; !ok && i < d->transports.size(); ++i)
		ok |= set_contains_list(features, d->transports.at(i)->features());
	if (!ok) return false;
	ok = false;
	for (int i = 0; !ok && i < d->descriptions.size(); ++i)
		ok |= set_contains_list(features, d->descriptions.at(i)->features());
	return ok;
}

JingleSession *JingleManager::createSession(const JID &responder, const QStringList &contents)
{
	JingleSession *session = new JingleSession(responder, contents, d_func()->client);
	emit sessionCreated(session);
	return session;
}

}

#include "jinglemanager.moc"
