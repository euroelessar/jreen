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
	JingleSessionContent() : contentObject(0), initiating(0) {}
	JingleSessionContent(const Content &o) : Content(o), contentObject(0), initiating(0) {}
	JingleSessionContent(const JingleSessionContent &o)
	    : Content(o), contentObject(o.contentObject), initiating(o.initiating) {}

	JingleContent *contentObject;
	int initiating : 1;
};

class JingleSessionPrivate
{
public:
	Q_DECLARE_PUBLIC(JingleSession);
	JingleSessionPrivate() : needMore(0), incoming(1), initiating(1) {}
	JingleSession *q_ptr;
	Client *client;
	JID other;
	QString sid;
	QList<JingleSessionContent> contents;
	int needMore : 30;
	int incoming : 1;
	int initiating : 1;

	JingleSessionContent *findContent(const QString &name);
	JingleSessionContent *findContent(JingleContent *content);
	void handle(const Jingle::Ptr &jingle);
	void accept(const JingleSessionContent &content);
	void add(const JingleSessionContent &content);
	void onTransportsReady(JingleContent *content, const QList<JingleTransport*> &transports);
	static JingleSessionPrivate *get(JingleSession *q) { return q->d_func(); }
};

}

#endif // JINGLESESSION_P_H
