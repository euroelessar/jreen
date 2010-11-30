/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef VCARD_P_H
#define VCARD_P_H

#include "vcard.h"
#include "jid.h"
#include <QUrl>

namespace jreen
{
	class VCardPrivate
	{
	public:
		QString formattedName;
		QString nickname;
		QDateTime bday;
		QUrl url;
		JID jabberId;
		QString title;
		QString role;
		QString note;
		QString description;
		QString mailer;
		QString timeZone;
		QString productID;
		QString revision;
		QString sortString;
		QString userID;
		VCard::Classification classification;
		VCard::Name name;
		VCard::Photo photo;
		QList<VCard::Telephone> telephones;
		QList<VCard::EMail> emails;
		
		static VCardPrivate *get(VCard *v) { return v->d_func(); }
	};
}

#endif // VCARD_P_H
