/****************************************************************************
 *  vcard.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "vcard_p.h"

namespace Jreen {

VCard::Name::Name() : d_ptr(new VCard::NamePrivate)
{
}

VCard::Name::Name(const VCard::Name &o) : d_ptr(o.d_ptr)
{
}

VCard::Name::Name(VCard::NamePrivate &p) : d_ptr(&p)
{
}

VCard::Name::~Name()
{
}

VCard::Name &VCard::Name::operator =(const VCard::Name &o)
{
	d_ptr = o.d_ptr;
	return *this;
}

QString VCard::Name::family() const
{
	return d_ptr->family;
}

void VCard::Name::setFamily(const QString &name)
{
	d_ptr->family = name;
}

QString VCard::Name::given() const
{
	return d_ptr->given;
}

void VCard::Name::setGiven(const QString &name)
{
	d_ptr->given = name;
}

QString VCard::Name::middle() const
{
	return d_ptr->middle;
}

void VCard::Name::setMiddle(const QString &name)
{
	d_ptr->middle = name;
}

QString VCard::Name::prefix() const
{
	return d_ptr->prefix;
}

void VCard::Name::setPrefix(const QString &name)
{
	d_ptr->prefix = name;
}

QString VCard::Name::suffix() const
{
	return d_ptr->suffix;
}

void VCard::Name::setSuffix(const QString &name)
{
	d_ptr->suffix = name;
}

VCard::Photo::Photo() : d_ptr(new PhotoPrivate)
{
}

VCard::Photo::Photo(const Photo &o) : d_ptr(o.d_ptr)
{
}

VCard::Photo::Photo(VCard::PhotoPrivate &p) : d_ptr(&p)
{
}

VCard::Photo::~Photo()
{
}

VCard::Photo &VCard::Photo::operator =(const Photo &o)
{
	d_ptr = o.d_ptr;
	return *this;
}

QString VCard::Photo::external() const
{
	return d_ptr->extval;
}

void VCard::Photo::setExternal(const QString &extval)
{
	d_ptr->extval = extval;
	d_ptr->binval = QByteArray();
	d_ptr->type = QString();
}

void VCard::Photo::setData(const QByteArray &data, const QString &mimeType)
{
	d_ptr->extval = QString();
	d_ptr->binval = data;
	d_ptr->type = mimeType;
}

QByteArray VCard::Photo::data() const
{
	return d_ptr->binval;
}

QString VCard::Photo::mimeType() const
{
	return d_ptr->type;
}

VCard::Telephone::Telephone() : d_ptr(new VCard::TelephonePrivate)
{
}

VCard::Telephone::Telephone(const Telephone &o) : d_ptr(o.d_ptr)
{
}

VCard::Telephone::Telephone(VCard::TelephonePrivate &p) : d_ptr(&p)
{
}

VCard::Telephone::~Telephone()
{
}

VCard::Telephone &VCard::Telephone::operator =(const VCard::Telephone &o)
{
	d_ptr = o.d_ptr;
	return *this;
}

bool VCard::Telephone::testType(Type t) const
{
	return d_ptr->types & t;
}

bool VCard::EMail::testType(Type t) const
{
	return d_ptr->types & t;
}

const QString &VCard::EMail::userId() const
{
	return d_ptr->userId;
}

void VCard::EMail::setType(Type t, bool value)
{
	d_ptr->types ^= ((d_ptr->types & t) == t) == value ? 0 : t;
}

void VCard::EMail::setUserId(const QString &userId)
{
	d_ptr->userId = userId;
}

void VCard::Telephone::setType(Type t, bool value)
{
	d_ptr->types ^= ((d_ptr->types & t) == t) == value ? 0 : t;
}

QString VCard::Telephone::number() const
{
	return d_ptr->number;
}

void VCard::Telephone::setNumber(const QString &number)
{
	d_ptr->number = number;
}

VCard::EMail::EMail() : d_ptr(new VCard::EMailPrivate)
{
}

VCard::EMail::EMail(const EMail &o) : d_ptr(o.d_ptr)
{
}

VCard::EMail::EMail(VCard::EMailPrivate &p) : d_ptr(&p)
{
}

VCard::EMail::~EMail()
{
}

VCard::EMail &VCard::EMail::operator =(const EMail &o)
{
	d_ptr = o.d_ptr;
	return *this;
}

VCard::Address::Address() : d_ptr(new VCard::AddressPrivate)
{
}

VCard::Address::Address(const Address &o) : d_ptr(o.d_ptr)
{
}

VCard::Address::Address(VCard::AddressPrivate &p) : d_ptr(&p)
{
}

VCard::Address::~Address()
{
}

VCard::Address &VCard::Address::operator =(const VCard::Address &o)
{
	d_ptr = o.d_ptr;
	return *this;
}

VCard::VCard(const QString &formattedName, Classification classification)
	: d_ptr(new VCardPrivate)
{
	Q_D(VCard);
	d->formattedName = formattedName;
	d->classification = classification;
}

VCard::VCard(VCardPrivate &p) : d_ptr(&p)
{
}

VCard::~VCard()
{
}

void VCard::setFormattedName(const QString &fn)
{
	d_func()->formattedName = fn;
}

const QString &VCard::formattedName() const
{
	return d_func()->formattedName;
}

void VCard::setName(const Name &name)
{
	d_func()->name = name;
}

const VCard::Name& VCard::name() const
{
	return d_func()->name;
}

void VCard::setNickname(const QString& nickname)
{
	d_func()->nickname = nickname;
}

QString VCard::nickname() const
{
	return d_func()->nickname;
}

void VCard::setName(const QString &family,
					const QString &given,
					const QString &middle,
					const QString &prefix,
					const QString &suffix)
{
	Q_D(VCard);
	d->name.setFamily(family);
	d->name.setGiven(given);
	d->name.setMiddle(middle);
	d->name.setPrefix(prefix);
	d->name.setSuffix(suffix);
}

void VCard::setPhoto(const QString &extval)
{
	Q_D(VCard);
	d->photo.setExternal(extval);
}

void VCard::setPhoto(const Photo &photo)
{
	d_func()->photo = photo;
}

VCard::Photo VCard::photo() const
{
	return d_func()->photo;
}

void VCard::setBday(const QDateTime& bday)
{
	d_func()->bday = bday;
}

QDateTime VCard::bday() const
{
	return d_func()->bday;
}

const QUrl &VCard::url() const
{
	return d_func()->url;
}

QList<VCard::Telephone> VCard::telephones() const
{
	return d_func()->telephones;
}

void VCard::addTelephone(const Telephone &t)
{
	d_func()->telephones.append(t);
}

void VCard::addEmail(const EMail &email)
{
	d_func()->emails.append(email);
}

QList<VCard::EMail> VCard::emails() const
{
	return d_func()->emails;
}

//AddressList& VCard::addresses() const
//{
//	return d_func()->addresses;
//}

//void VCard::addAdress(const Address &adr)
//{
//	return d_func()->addresses.append(adr);
//}

void VCard::setDesc(const QString& desc)
{
	d_func()->description = desc;
}

const QString& VCard::desc() const
{
	return d_func()->description;
}

} // namespace Jreen
