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

#include "prep_p.h"
#include <QLibrary>
#include <QStringList>

#define JID_PORTION_SIZE 1023

#ifdef HAVE_IDN
#include <stringprep.h>

#define Jreen_idn_stringprep                   stringprep
#define Jreen_idn_stringprep_nameprep          stringprep_nameprep
#define Jreen_idn_stringprep_xmpp_nodeprep     stringprep_xmpp_nodeprep
#define Jreen_idn_stringprep_xmpp_resourceprep stringprep_xmpp_resourceprep
namespace Jreen
{
	static bool loadLibIDN() { return true; }
}
#else
namespace Jreen
{
	typedef void Stringprep_profile;
	typedef int Stringprep_profile_flags;
	static void *Jreen_idn_stringprep_nameprep = 0;
	static void *Jreen_idn_stringprep_xmpp_nodeprep = 0;
	static void *Jreen_idn_stringprep_xmpp_resourceprep = 0;
	typedef int (*Jreen_idn_stringprep_) (char *in, size_t maxlen, Stringprep_profile_flags flags,
	                                      const Stringprep_profile *profile);
	static Jreen_idn_stringprep_ Jreen_idn_stringprep = 0;
	static bool triedToInit = false;
	
	static bool loadLibIDN()
	{
		if (Jreen_idn_stringprep)
			return true;
		if (triedToInit)
			return false;
		triedToInit = true;
		QLibrary lib(QLatin1String("idn"));
		if (!lib.load()) {
#ifdef Q_OS_WIN32
			QStringList paths = QStringList()
			        << QLatin1String("idn-11")
			        << QLatin1String("libidn-11")
			        << QLatin1String("libidn");
			bool ok = false;
			for (int i = 0; !ok && i < paths.size(); i++) {
				lib.setFileName(paths.at(i));
				ok |= lib.load();
			}
			if (!ok)
#endif // Q_OS_WIN32
				return false;
		}
		Jreen_idn_stringprep_nameprep = lib.resolve("stringprep_nameprep");
		Jreen_idn_stringprep_xmpp_nodeprep = lib.resolve("stringprep_xmpp_nodeprep");
		Jreen_idn_stringprep_xmpp_resourceprep = lib.resolve("stringprep_xmpp_resourceprep");
		Jreen_idn_stringprep = reinterpret_cast<Jreen_idn_stringprep_>(lib.resolve("stringprep"));
		return true;
	}
}
#endif // HAVE_IDN

namespace Jreen
{
	static QString prepare(const QString &s, bool *ok, const Stringprep_profile *profile)
	{
		if (s.isEmpty() || s.size() > JID_PORTION_SIZE) {
			*ok = false;
			return QString();
		}
		QByteArray in = s.toUtf8();
		in.resize(JID_PORTION_SIZE);
		int rc = Jreen_idn_stringprep(in.data(), JID_PORTION_SIZE,
		                              static_cast<Stringprep_profile_flags>(0),
		                              profile);
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
		return prepare(node, ok, Jreen_idn_stringprep_xmpp_nodeprep);
	}
	
	QString Prep::namePrep(const QString &domain, bool *ok)
	{
		if (!loadLibIDN()) {
			*ok = true;
			return domain.toLower();
		}
		return prepare(domain, ok, Jreen_idn_stringprep_nameprep);
	}
	
	QString Prep::resourcePrep(const QString &resource, bool *ok)
	{
		if (!loadLibIDN()) {
			*ok = true;
			return resource;
		}
		return prepare(resource, ok, Jreen_idn_stringprep_xmpp_resourceprep);
	}
	
}
