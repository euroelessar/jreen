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

#include "jinglesession_p.h"
#include "../client.h"
#include "../util.h"
#include "../logger.h"

namespace Jreen
{

JingleSessionContent *JingleSessionPrivate::findContent(const QString &name)
{
	for (int i = 0; i < contents.size(); ++i) {
		if (contents.at(i).name == name)
			return &contents[i];
	}
	return 0;
}

JingleSessionContent *JingleSessionPrivate::findContent(JingleContent *content)
{
	for (int i = 0; i < contents.size(); ++i) {
		if (contents[i].contentObject == content)
			return &contents[i];
	}
	return 0;
}

void JingleSessionPrivate::handle(const Jingle::Ptr &jingle)
{
	Logger::debug() << Q_FUNC_INFO;
	if (jingle->action == Jingle::SessionAccept) {
		foreach (const Jingle::Content &remoteContent, jingle->contents) {
			JingleSessionContent *content = findContent(remoteContent.name);
			if (!content) {
				qWarning("Unknown content with name %s", qPrintable(content->name));
				continue;
			}
			if (!remoteContent.description) {
				qWarning("Unknown content description with name %s", qPrintable(content->name));
				continue;
			}
			content->contentObject->handleDescription(remoteContent.description);
			if (remoteContent.transports.size() != 1) {
				qWarning("Content %s has %d transports", qPrintable(content->name),
				         remoteContent.transports.size());
			}
			if (!remoteContent.transports.value(0)) {
				qCritical("Content %s has no transports", qPrintable(content->name));
				continue;
			}
			JingleContentPrivate *p = JingleContentPrivate::get(content->contentObject);
			const JingleTransportInfo::Ptr &transportInfo = remoteContent.transports.at(0);
			for (int i = content->transports.size() - 1; i >= 0; --i) {
				if (content->transports[i]->payloadType() != transportInfo->payloadType()) {
					content->transports.removeAt(i);
					delete p->transports.takeAt(i);
				}
			}
			if (p->transports.isEmpty()) {
				qCritical("Content %s has no needed transport", qPrintable(content->name));
				continue;
			}
			p->setTransport(p->transports.first());
			p->transport->setRemoteInfo(transportInfo, true);
		}
	} else if (jingle->action == Jingle::SessionTerminate) {
		emit q_func()->terminated();
	} else if (jingle->action == Jingle::ContentAdd) {
		foreach (const Jingle::Content &remoteContent, jingle->contents) {
			if (!remoteContent.description || remoteContent.transports.isEmpty()) {
				Jingle::send(q_func(), Jingle::ContentReject, remoteContent);
				continue;
			}
			JingleSessionContent content = remoteContent;
			content.creator = Jingle::Initiator;
			JingleManagerPrivate *manager = JingleManagerPrivate::get(client->jingleManager());
			content.contentObject = manager->content(remoteContent.description, q_func());
			if (!content.contentObject) {
				Jingle::send(q_func(), Jingle::ContentReject, remoteContent);
				continue;
			}
			content.description = content.contentObject->handleDescription(remoteContent.description);
			if (!content.description) {
				content.description = content.contentObject->defaultDescription();
				Jingle::send(q_func(), Jingle::ContentReject, content);
				continue;
			}
			JingleContentPrivate *contentD = JingleContentPrivate::get(content.contentObject);
			contentD->needAccept = 1;
			contents << content;
			JingleContentPrivate::get(content.contentObject)->initiateTransports(content.transports);
			emit q_func()->contentAdded(content.contentObject);
		}
	}
}

void JingleSessionPrivate::accept(const JingleSessionContent &content)
{
	IQReply *reply = Jingle::send(q_func(), Jingle::ContentAccept, content);
	Q_UNUSED(reply);
}

void JingleSessionPrivate::add(const JingleSessionContent &content)
{
	IQReply *reply = Jingle::send(q_func(), Jingle::ContentAdd, content);
	Q_UNUSED(reply);
}

void JingleSessionPrivate::onTransportsReady(JingleContent *content, const QList<JingleTransport*> &transports)
{
	for (int i = 0; i < contents.size(); ++i) {
		if (contents.at(i).contentObject != content)
			continue;
		JingleSessionContent &sessionContent = contents[i];
		for (int j = 0; j < transports.size(); ++j)
			sessionContent.transports << transports[j]->localInfo();
		if (initiating) {
			needMore--;
			if (needMore == 0)
				q_func()->initiate();
		} else {
			JingleContentPrivate *p = JingleContentPrivate::get(content);
			if (p->canAccept)
				accept(sessionContent);
			else
				add(sessionContent);
		}
	}
}

JingleSession::JingleSession(const JID &responder, const QStringList &contents, Client *client)
    : QObject(client->jingleManager()), d_ptr(new JingleSessionPrivate)
{
	Q_D(JingleSession);
	d->q_ptr = this;
	d->incoming = 0;
	d->client = client;
	d->other = responder;
	d->sid = Util::randomStringHash(16);
	JingleManagerPrivate *manager = JingleManagerPrivate::get(client->jingleManager());
	manager->sessions.insert(d->sid, this);
	manager->sessionsByJid.insert(responder, this);
	for (int i = 0; i < contents.size(); ++i)
		addContent(contents.at(i));
	if (d->needMore == 0 && d->contents.size() > 0)
		initiate();
}

JingleSession::JingleSession(const Payload::Ptr &payload, Client *client)
    : QObject(client->jingleManager()), d_ptr(new JingleSessionPrivate)
{
	Q_D(JingleSession);
	Q_ASSERT(se_cast<Jingle*>(payload.data()));
	d->client = client;
	d->initiating = 0;
	Jingle::Ptr jingle = payload.staticCast<Jingle>();
	d->other = jingle->initiator;
	d->sid = jingle->sid;
	JingleManagerPrivate *manager = JingleManagerPrivate::get(client->jingleManager());
	manager->sessions.insert(d->sid, this);
	manager->sessionsByJid.insert(jingle->initiator, this);
}

void JingleSession::initiate()
{
	Q_D(JingleSession);
	IQReply *reply = Jingle::send(this, Jingle::SessionInitiate, d->contents);
	Q_UNUSED(reply);
}

void JingleSession::terminate()
{
	IQReply *reply = Jingle::send(this, Jingle::SessionTerminate);
	connect(reply, SIGNAL(received(Jreen::IQ)), SIGNAL(terminated()));
}

bool JingleSession::isIncoming() const
{
	return d_func()->incoming;
}

JID JingleSession::jid() const
{
	return d_func()->other;
}

JingleContent *JingleSession::content(const QString &id) const
{
	Q_D(const JingleSession);
	for (int i = 0; i < d->contents.size(); ++i) {
		if (d->contents.at(i).name == id)
			return d->contents.at(i).contentObject;
	}
	return 0;
}

QStringList JingleSession::contents() const
{
	Q_D(const JingleSession);
	QStringList result;
	for (int i = 0; i < d->contents.size(); ++i)
		result << d->contents.at(i).name;
	return result;
}

JingleSession::~JingleSession()
{
	Q_D(JingleSession);
	JingleManagerPrivate *manager = JingleManagerPrivate::get(d->client->jingleManager());
	manager->sessions.remove(d->sid);
	manager->sessionsByJid.remove(d->other);
}

bool JingleSession::addContent(const QString &media, const QString &id)
{
	Q_D(JingleSession);
	JingleManagerPrivate *manager = JingleManagerPrivate::get(d->client->jingleManager());
	JingleSessionContent content;
	content.creator = Jingle::Initiator;
	content.contentObject = manager->content(media, this);
	if (!content.contentObject) {
		qWarning("Unknown content %s", qPrintable(media));
		return false;
	}
	content.description = content.contentObject->defaultDescription();
	content.name = id.isEmpty() ? Util::randomStringHash(8) : id;
	JingleContentPrivate *contentD = JingleContentPrivate::get(content.contentObject);
	contentD->initiateTransports();
	d->contents << content;
	if (d->initiating)
		d->needMore++;
	emit contentAdded(content.contentObject);
	return true;
}

void JingleSession::accept()
{
	Q_D(JingleSession);
	for (int i = 0; i < d->contents.size(); ++i) {
		JingleContentPrivate *content = JingleContentPrivate::get(d->contents.at(i).contentObject);
		if (content->needAccept)
			content->accept();
	}
}

void JingleSession::decline()
{
	Q_D(JingleSession);
	for (int i = 0; i < d->contents.size(); ++i) {
		JingleContentPrivate *content = JingleContentPrivate::get(d->contents.at(i).contentObject);
		if (content->needAccept)
			content->decline();
	}
}

}

#include "moc_jinglesession.cpp"
