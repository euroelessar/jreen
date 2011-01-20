/****************************************************************************
 *  prep.cpp
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

#include "prep.h"
#include <QLibrary>

#define JID_PORTION_SIZE 1023

namespace jreen
{
	static void *_idn_stringprep_nameprep = 0;
	static void *_idn_stringprep_xmpp_nodeprep = 0;
	static void *_idn_stringprep_xmpp_resourceprep = 0;
	typedef int (*_idn_stringprep_) (char *in, size_t maxlen, int flags, void *profile);
	static _idn_stringprep_ _idn_stringprep = 0;
	
	static bool loadLibIDN()
	{
		if (_idn_stringprep)
			return true;
		QLibrary lib(QLatin1String("idn"));
		if (!lib.load())
			return false;
		_idn_stringprep_nameprep = lib.resolve("stringprep_nameprep");
		_idn_stringprep_xmpp_nodeprep = lib.resolve("stringprep_xmpp_nodeprep");
		_idn_stringprep_xmpp_resourceprep = lib.resolve("stringprep_xmpp_resourceprep");
		_idn_stringprep = reinterpret_cast<_idn_stringprep_>(lib.resolve("stringprep"));
		return true;
	}
	
	static QString prepare(const QString &s, bool *ok, void *profile)
	{
		if (s.isEmpty() || s.size() > JID_PORTION_SIZE) {
			*ok = false;
			return QString();
		}
		QByteArray in = s.toUtf8();
		in.resize(JID_PORTION_SIZE);
		int rc = _idn_stringprep(in.data(), JID_PORTION_SIZE, 0, profile);
		*ok = rc == 0;
		if (*ok)
			return QString::fromUtf8(in);
		else
			return QString();
	}
	
	QString Prep::nodePrep(const QString &node, bool *ok)
	{
		if (!loadLibIDN()) {
			*ok = true;
			return node.toLower();
		}
		return prepare(node, ok, _idn_stringprep_xmpp_nodeprep);
	}
	
	QString Prep::namePrep(const QString &domain, bool *ok)
	{
		if (!loadLibIDN()) {
			*ok = true;
			return domain.toLower();
		}
		return prepare(domain, ok, _idn_stringprep_nameprep);
	}
	
	QString Prep::resourcePrep(const QString &resource, bool *ok)
	{
		if (!loadLibIDN()) {
			*ok = true;
			return resource;
		}
		return prepare(resource, ok, _idn_stringprep_xmpp_resourceprep);
	}
	
}
