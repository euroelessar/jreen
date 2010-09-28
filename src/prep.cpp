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

#include <stringprep.h>
#include <idna.h>

#define JID_PORTION_SIZE 1023

J_BEGIN_NAMESPACE

static QString prepare( const QString &s, bool *ok, const Stringprep_profile* profile )
{
	if( s.isEmpty() || s.size() > JID_PORTION_SIZE )
	{
		*ok = false;
		return QString();
	}

	QByteArray in = s.toUtf8();
	in.resize(JID_PORTION_SIZE);
	int rc = stringprep( in.data(), JID_PORTION_SIZE, (Stringprep_profile_flags)0, profile );
	*ok = rc == STRINGPREP_OK;
	if(*ok)
		return QString::fromUtf8(in);
	else
		return QString();
}

QString Prep::nodePrep( const QString &node, bool *ok )
{
	return prepare( node, ok, stringprep_xmpp_nodeprep );
}

QString Prep::namePrep( const QString &domain, bool *ok )
{
	return prepare( domain, ok, stringprep_nameprep );
}

QString Prep::resourcePrep( const QString &resource, bool *ok )
{
	return prepare( resource, ok, stringprep_xmpp_resourceprep );
}

QString Prep::idna( const QString &domain, bool *ok )
{
	if( domain.isEmpty() || domain.size() > JID_PORTION_SIZE )
	{
		if(ok) *ok = false;
		return QString();
	}
	QByteArray in = domain.toUtf8();
	QByteArray out;
	char *data = out.data();

	int rc = idna_to_ascii_8z( in.constData(), &data, (Idna_flags)IDNA_USE_STD3_ASCII_RULES );
	//int rc = idna_to_unicode_8z8z( in.constData(), &data, (Idna_flags)IDNA_USE_STD3_ASCII_RULES );
	if( rc == IDNA_SUCCESS )
	{
		if(ok) *ok = true;
		return QString::fromUtf8(out);
	}
	if(ok) *ok = false;
	return QString();
}

J_END_NAMESPACE
