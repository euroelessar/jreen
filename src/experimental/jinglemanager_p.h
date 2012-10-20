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

#ifndef JINGLEMANAGER_P_H
#define JINGLEMANAGER_P_H

#include "jinglemanager.h"
#include "jinglefactory_p.h"

namespace Jreen
{

class JingleManagerPrivate
{
public:
	Client *client;
	QList<AbstractJingleTransportFactory*> transports;
	QList<AbstractJingleContentFactory*> descriptions;
	QHash<QString, JingleSession*> sessions;
	QHash<JID, JingleSession*> sessionsByJid;
	
	JingleContent *content(const QString &name, JingleSession *session);
	JingleContent *content(const JingleDescription::Ptr &decription, JingleSession *session);
	JingleTransport *transport(const JingleTransportInfo::Ptr &info, JingleContent *content);
	void _q_iqReceived(const Jreen::IQ &iq);
	static JingleManagerPrivate *get(JingleManager *q) { return q->d_func(); }
};

}

#endif // JINGLEMANAGER_P_H
