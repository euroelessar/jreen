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

#ifndef JINGLECONTENT_P_H
#define JINGLECONTENT_P_H

#include "jinglecontent.h"

namespace Jreen
{

class JingleContentPrivate
{
	Q_DECLARE_PUBLIC(JingleContent)
public:
	JingleContentPrivate(JingleContent *q)
	    : q_ptr(q), transport(0), componentCount(1), needTransports(0),
	      needAccept(0), canAccept(0), state(JingleContent::Gathering) {}
	
	JingleContent *q_ptr;
	JingleSession *session;
	JingleTransport *transport;
	int componentCount : 14;
	int needTransports : 14;
	int needAccept : 1;
	int canAccept : 1;
	JingleContent::State state;
	QList<JingleTransport*> transports;
	QList<JingleTransportInfo::Ptr> transportInfos;
	
	void _q_received(int component, const QByteArray &data);
	virtual void _q_stateChanged(Jreen::JingleTransport::State);
	void _q_localInfoReady(const Jreen::JingleTransportInfo::Ptr &);
	void _q_tryStateChanged(Jreen::JingleTransport::State state);
	void setTransport(JingleTransport *trueTransport);
	void initiateTransports();
	void tryNextTransport();
	void initiateTransports(const QList<JingleTransportInfo::Ptr> &transportInfos);
	void accept();
	void decline();
	static JingleContentPrivate *get(JingleContent *q) { return q->d_func(); }
};

}

#endif // JINGLECONTENT_P_H
