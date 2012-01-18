/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef JID_H
#define JID_H

#include "jreen.h"
#include <QSharedData>
#include <QMetaType>

namespace Jreen
{

class JIDData;
class JREEN_EXPORT JID
{
public:
	JID();
	JID(const JID &other);
	JID(const QString &jid);
	JID(const QLatin1String &jid);
	JID(const QString &node, const QString &domain, const QString &resource = QString());
	virtual ~JID();

	bool setJID(const QString &jid);
	inline void clear() { setJID(QString()); }

	const QString &domain() const;
	const QString &node() const;
	const QString &resource() const;
	const QString &bare() const;
	const QString &full() const;

	JID bareJID() const;

	bool setNode(const QString &node);
	bool setDomain(const QString &domain);
	bool setResource(const QString &resource);

	bool isBare() const;
	bool isFull() const;
	bool isDomain() const;

	JID withNode(const QString &node) const;
	JID withResource(const QString &resource) const;

	JID &operator =(const QString &s);
	JID &operator =(const QLatin1String &s);
	JID &operator =(const JID &other);

	bool operator ==(const QString &right) const;
	bool operator !=(const QString &right) const;
	bool operator ==(const QLatin1String &right) const;
	bool operator !=(const QLatin1String &right) const;
	bool operator ==(const JID& right) const;
	bool operator !=(const JID& right) const;

	bool isValid() const;
	operator QString() const;
private:
	QSharedDataPointer<JIDData> d_ptr;
	friend class JIDData;
};

}

inline uint qHash(const Jreen::JID &jid) { return qHash(jid.full()); }

Q_DECLARE_METATYPE(Jreen::JID)

#endif // JID_H
