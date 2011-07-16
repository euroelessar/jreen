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

#ifndef JINGLE_H
#define JINGLE_H

#include "stanzaextension.h"
#include "jid.h"
#include "jingletransport.h"
#include "jinglecontent.h"

namespace Jreen
{

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
	
	JID initiator;
	JID responder;
	QString sid;
	Action action;
	QList<Content> contents;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(Jreen::Jingle::SendersType)

#endif // JINGLE_H
