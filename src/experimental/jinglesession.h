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

#ifndef JINGLESESSION_H
#define JINGLESESSION_H

#include "jinglecontent.h"
#include "../stanzaextension.h"

namespace Jreen
{

class JingleManager;
class JingleManagerPrivate;
class JingleSessionPrivate;
class Client;
class JID;

class JREEN_EXPORT JingleSession : public QObject
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JingleSession)
public:
//	enum State { };
	
	~JingleSession();

	bool addContent(const QString &content, const QString &id = QString());
	void initiate();
	void terminate();
	void accept();
	void decline();
	bool isIncoming() const;
	JID jid() const;
	JingleContent *content(const QString &id) const;
	QStringList contents() const;
	
signals:
	void contentAdded(Jreen::JingleContent *content);
	void contentRemoved(Jreen::JingleContent *content);
	void terminated();
	
protected:
    JingleSession(const JID &responder, const QStringList &contents, Client *client);
	JingleSession(const Payload::Ptr &payload, Client *client);
	friend class JingleManager;
	friend class JingleManagerPrivate;
	QScopedPointer<JingleSessionPrivate> d_ptr;
};

}

#endif // JINGLESESSION_H
