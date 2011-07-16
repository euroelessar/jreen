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

#ifndef JINGLEMANAGER_H
#define JINGLEMANAGER_H

#include "iq.h"

namespace Jreen
{

class Client;
class JingleSession;
class JingleManagerPrivate;

class JREEN_EXPORT JingleManager : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JingleManager)
public:
	~JingleManager();
	
	bool checkSupport(const QSet<QString> &features);
	JingleSession *createSession(const JID &responder, const QStringList &contents);
	
signals:
	void sessionCreated(Jreen::JingleSession *session);
	
protected:
    JingleManager(Client *client);
	friend class Client;
	QScopedPointer<JingleManagerPrivate> d_ptr;
	Q_PRIVATE_SLOT(d_func(), void _q_iqReceived(const Jreen::IQ &))
};

}

#endif // JINGLEMANAGER_H
