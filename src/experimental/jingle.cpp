/****************************************************************************
**
** Jreen
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "jingle_p.h"
#include "jinglesession_p.h"
#include "../iqreply.h"

namespace Jreen
{

Jingle::Jingle() : action(Jingle::SessionInitiate)
{
}

Jingle::Ptr Jingle::create(JingleSession *session, Action action)
{
	Jingle::Ptr jingle = Jingle::Ptr::create();
	JingleSessionPrivate *d = JingleSessionPrivate::get(session);
	jingle->initiator = d->client->jid();
//	jingle->responder = d->other;
	if (d->incoming)
		qSwap(jingle->initiator, jingle->responder);
	jingle->action = action;
	jingle->sid = d->sid;
	return jingle;
}

IQReply *Jingle::send(JingleSession *session, Action action, const QList<Content> &contents)
{
	JingleSessionPrivate *d = JingleSessionPrivate::get(session);
	Jingle::Ptr jingle = create(session, action);
	jingle->contents = contents;
	IQ iq(IQ::Set, d->other);
	iq.addExtension(jingle);
	return d->client->send(iq);
}

IQReply *Jingle::send(JingleSession *session, Action action, const Content &content)
{
	return send(session, action, QList<Content>() << content);
}

IQReply *Jingle::send(JingleSession *session, Action action, JingleContent *contentObject)
{
	JingleSessionPrivate *d = JingleSessionPrivate::get(session);
	JingleSessionContent *content = d->findContent(contentObject);
	Q_ASSERT(content);
	return send(session, action, *content);
}

}
