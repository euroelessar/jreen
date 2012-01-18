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

#ifndef VCARD_P_H
#define VCARD_P_H

#include "vcard.h"
#include "jid.h"
#include <QUrl>
#include <QStringList>

namespace Jreen
{
class VCard::NamePrivate : public QSharedData
{
public:
	NamePrivate() {}
	NamePrivate(NamePrivate &o) :
		QSharedData(o), family(o.family), given(o.given),
		middle(o.middle), prefix(o.prefix), suffix(o.suffix) {}

	QString family;
	QString given;
	QString middle;
	QString prefix;
	QString suffix;

	static const VCard::NamePrivate *get(const VCard::Name *o) { return o->d_ptr.data(); }
};

class VCard::PhotoPrivate : public QSharedData
{
public:
	PhotoPrivate() {}
	PhotoPrivate(PhotoPrivate &o) :
		QSharedData(o), extval(o.extval), binval(o.binval), type(o.type) {}

	QString extval;
	QByteArray binval;
	QString type;

	static const VCard::PhotoPrivate *get(const VCard::Photo *o) { return o->d_ptr.data(); }
};

class VCard::TelephonePrivate : public QSharedData
{
public:
	TelephonePrivate() : types(0)  {}
	TelephonePrivate(TelephonePrivate &o) : QSharedData(o), types(o.types), number(o.number) {}

	int types;
	QString number;

	static const VCard::TelephonePrivate *get(const VCard::Telephone *o) { return o->d_ptr.data(); }
};

class VCard::EMailPrivate : public QSharedData
{
public:
	EMailPrivate() : types(0) {}
	EMailPrivate(EMailPrivate &o) : QSharedData(o), types(o.types), userId(o.userId) {}

	int types;
	QString userId;

	static const VCard::EMailPrivate *get(const VCard::EMail *o) { return o->d_ptr.data(); }
};

class VCard::AddressPrivate : public QSharedData
{
public:
	AddressPrivate() : types(0) {}
	AddressPrivate(AddressPrivate &o) :
		QSharedData(o), types(o.types), pobox(o.pobox),
		extendedAddress(o.extendedAddress), street(o.street),
		locality(o.locality), region(o.region), pcode(o.pcode) ,
		country(o.country) {}

	int types;
	QString pobox;
	QString extendedAddress;
	QString street;
	QString locality;
	QString region;
	QString pcode;
	QString country;

	static const VCard::AddressPrivate *get(const VCard::Address *o) { return o->d_ptr.data(); }
};

class VCard::OrganizationPrivate : public QSharedData
{
public:
	QString orgName;
	QStringList orgUnits;
};

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
	VCard::Organization org;
	QList<VCard::Telephone> telephones;
	QList<VCard::EMail> emails;
	QList<VCard::Address> addresses;

	static VCardPrivate *get(VCard *v) { return v->d_func(); }
};
}

#endif // VCARD_P_H
