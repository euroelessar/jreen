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

#ifndef JINGLESESSION_P_H
#define JINGLESESSION_P_H

#include "jinglesession.h"
#include "jinglemanager_p.h"
#include "jingle_p.h"
#include "jinglecontent_p.h"

namespace Jreen
{

class JingleSessionContent : public Jingle::Content
{
public:
	JingleSessionContent() : needMore(0) {}

	JingleContent *contentObject;
	QList<JingleTransport*> transportObjects;
	int needMore;
};

class JingleSessionPrivate
{
public:
	Q_DECLARE_PUBLIC(JingleSession);
	JingleSessionPrivate() : needMore(0), incoming(1) {}
	JingleSession *q_ptr;
	Client *client;
	JID other;
	QString sid;
	QList<JingleSessionContent> contents;
	int needMore : 31;
	int incoming : 1;

	JingleSessionContent *findContent(const QString &name);
	void handle(const Jingle::Ptr &jingle);
	void _q_localInfoReady(const Jreen::JingleTransportInfo::Ptr &);
	static JingleSessionPrivate *get(JingleSession *q) { return q->d_func(); }
};

}

#endif // JINGLESESSION_P_H
