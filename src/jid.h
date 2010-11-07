/****************************************************************************
 *  jid.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef JID_H
#define JID_H

#include "jreen.h"

namespace jreen
{

class JIDPrivate;

class JREEN_EXPORT JID
{
public:
	JID();
	JID(const JID &jid);
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

	inline JID bareJID() const { return JID(bare()); }

	bool setNode(const QString &node);
	bool setDomain(const QString &domain);
	bool setResource(const QString &resource);

	// TODO: non-inline, don't want to rebuild jreen right now
	inline bool isBare() const { return isValid() && resource().isEmpty() && !node().isEmpty(); }
	inline bool isFull() const { return isValid() && !node().isEmpty() && !resource().isEmpty(); }
	inline bool isDomain() const { return isValid() && resource().isEmpty() && node().isEmpty(); }

	JID clone() const;
	JID withNode(const QString &node) const;
	JID withResource(const QString &resource) const;

	inline JID &operator =(const QString &s) { setJID(s); return *this; }
	inline JID &operator =(const QLatin1String &s) { setJID(QString(s)); return *this; }
	JID &operator =(const JID &jid);

	bool operator ==(const QString &right) const;
	bool operator !=(const QString &right) const;
	bool operator ==(const QLatin1String &right) const;
	bool operator !=(const QLatin1String &right) const;
	bool operator ==(const JID& right) const;
	bool operator !=(const JID& right) const;

	// ASCII compatibility
#ifndef QT_NO_CAST_FROM_ASCII
	inline QT_ASCII_CAST_WARN_CONSTRUCTOR JID(const char *jid) { impl = 0; operator =(QLatin1String(jid)); }
	inline QT_ASCII_CAST_WARN_CONSTRUCTOR JID &operator =(const char *s) { setJID(QLatin1String(s)); return *this; }
	inline QT_ASCII_CAST_WARN_CONSTRUCTOR bool operator ==(const char *right) const { return operator ==(QLatin1String(right)); }
	inline QT_ASCII_CAST_WARN_CONSTRUCTOR bool operator !=(const char *right) const { return operator !=(QLatin1String(right)); }
#endif

	bool isValid() const;
	operator QString() const;
private:
#ifdef QT_NO_CAST_FROM_ASCII
	JID(const char *jid);
	JID &operator =(const char *s);
	bool operator ==(const char *right);
	bool operator !=(const char *right);
#endif
	JIDPrivate *impl;
};

}

inline uint qHash(const jreen::JID &jid) { return qHash(jid.full()); }

#endif // JID_H
