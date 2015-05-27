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

#ifndef JREEN_JINGLE_P_H
#define JREEN_JINGLE_P_H

#include "../stanzaextension.h"
#include "../jid.h"
#include "jingletransport.h"
#include "jinglecontent.h"

namespace Jreen
{

class IQReply;

class Jingle : public Payload
{
	J_PAYLOAD(Jreen::Jingle)
public:
	enum Action {
		ContentAccept,
		ContentAdd,
		ContentModify,
		ContentReject,
		ContentRemove,
		DescriptionInfo,
		SecurityInfo,
		SessionAccept,
		SessionInfo,
		SessionInitiate,
		SessionTerminate,
		TransportAccept,
		TransportInfo,
		TransportReject,
		TransportReplace
	};
	
	enum CreatorType {
		Invalid = -1,
		None = 0,
		Initiator = 1,
		Responder = 2,
		Both = Initiator | Responder
	};
	
	typedef QFlags<CreatorType> SendersType;

	struct Content
	{
		Content() : creator(Initiator), senders(Both) {}
		QString name;
		CreatorType creator;
		SendersType senders;
		JingleDescription::Ptr description;
		QList<JingleTransportInfo::Ptr> transports;
	};
	
    Jingle();
	
	static Jingle::Ptr create(JingleSession *session, Action action);
	static IQReply *send(JingleSession *session, Action action, const QList<Content> &contents = QList<Content>());
	template <typename T>
	static IQReply *send(JingleSession *session, Action action, const QList<T> &list)
	{
		QList<Content> contents;
		for (int i = 0; i < list.size(); ++i)
			contents << list[i];
		return send(session, action, contents);
	}
	
	static IQReply *send(JingleSession *session, Action action, const Content &content);
	static IQReply *send(JingleSession *session, Action action, JingleContent *content);
	
	JID initiator;
	JID responder;
	QString sid;
	Action action;
	QList<Content> contents;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Jreen::Jingle::SendersType)

#endif // JREEN_JINGLE_P_H
