/****************************************************************************
 *  vcard.h
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

#ifndef VCARD_H
#define VCARD_H
#include "stanzaextension.h"
#include "nickname.h"
#include <QDateTime>

//implementation of XEP 0054
//http://xmpp.org/extensions/xep-0054.html
namespace jreen {

class VCardPrivate;

class JREEN_EXPORT VCard : public StanzaExtension
{
	J_EXTENSION(jreen::VCard,
		   "/iq/vcard[@xmlns='vcard-temp']")
	Q_DECLARE_PRIVATE(VCard)
public:
	/**
	* Addressing type indicators.
	* @note @c AddrTypeDom and @c AddrTypeIntl are mutually exclusive. If both are present,
	* @c AddrTypeDom takes precendence.
	* @note Also note that not all adress types are applicable everywhere. For example,
	* @c AddrTypeIsdn does not make sense for a postal address. Check XEP-0054
	* for details.
	*/
	enum AddressType
	{
		AddrTypeHome	=	0x1,		/**< Home address. */
		AddrTypeWork	=	0x2,		/**< Work address. */
		AddrTypePref	=	0x4,		/**< Preferred address. */
		AddrTypeX400	=	0x8,		/**< X.400 address. */
		AddrTypeInet	=	0x10,		/**< Internet address. */
		AddrTypeParcel	=	0x20,		/**< Parcel address. */
		AddrTypePostal	=	0x40,		/**< Postal address. */
		AddrTypeDom		=	0x80,		/**< Domestic(?) address. */
		AddrTypeIntl	=	0x100,		/**< International(?) address. */
		AddrTypeVoice	=	0x200,		/**< Voice number. */
		AddrTypeFax		=	0x400,		/**< Fax number. */
		AddrTypePager	=	0x800,		/**< Pager. */
		AddrTypeMsg		=	0x1000,		/**< MSG(?) */
		AddrTypeCell	=	0x2000,		/**< Cell phone number. */
		AddrTypeVideo	=	0x4000,		/**< Video chat(?). */
		AddrTypeBbs		=	0x8000,		/**< BBS. */
		AddrTypeModem	=	0x10000,	/**< Modem. */
		AddrTypeIsdn	=	0x20000,	/**< ISDN. */
		AddrTypePcs		=	0x40000		/**< PCS. */
	};
	/**
	* Classifies the VCard.
	*/
	enum Classification
	{
		ClassNone = 0,		/**< Not classified. */
		ClassPublic,		/**< Public. */
		ClassPrivate,		/**< Private. */
		ClassConfidential	/**< Confidential. */
	};

	class Name
	{
	public:
		QString family;         /**< Family name. */
		QString given;          /**< Given name. */
		QString middle;         /**< Middle name. */
		QString prefix;         /**< Name prefix. */
		QString suffix;         /**< Name suffix. */
	};
	class Photo
	{
	public:
		QString extval;         /**< The photo is not stored inside the VCard. This is a hint (URL?)
		  * where to look for it. */
		QString binval;         /**< This is the photo (base64). */
		QString type;           /**< This is a hint at the mime-type. May be forged! */
	};
	
	class Telephone
	{
	public:
		enum Type
		{
			Home      = 0x0001, 
			Work      = 0x0002,
			Voice     = 0x0004,
			Fax       = 0x0008,
			Pager     = 0x0010,
			Msg       = 0x0020,
			Cell      = 0x0040,
			Video     = 0x0080,
			BBS       = 0x0100,
			Modem     = 0x0200,
			ISDN      = 0x0400,
			PCS       = 0x0800,
			Preferred = 0x1000
		};
		typedef QFlags<Type> Types;
		int types;
		QString number;
	};
	class EMail
	{
	public:
		enum Type
		{
			Home      = 0x01,
			Work      = 0x02,
			Internet  = 0x04,
			Preferred = 0x08,
			X400      = 0x10,
		};
		int types;
		QString userId;
	};
	class Address
	{
	public:
		enum Type
		{
			Home          = 0x01,
			Work          = 0x02,
			Postal        = 0x04,
			Parcel        = 0x08,
			Domestic      = 0x10,
			International = 0x20,
			Preferred     = 0x40
		};
		int types;
		QString pobox;
		QString extendedAddress;
		QString street;
		QString locality;
		QString region;
		QString pcode;
		QString country;
	};

	VCard(const QString &formattedName = QString(), Classification classification = ClassNone);
	VCard(VCardPrivate &p);
	~VCard();
	/**
	* Sets the formatted name.
	* @param name The formatted name.
	*/
	void setFormattedName(const QString &fn) {m_formattedName = fn;}
	/**
	* Returns the formatted name.
	* @return The formatted name.
	*/
	const QString &formattedName() const {return m_formattedName;}
	inline void setName(const QString& family, const QString& given,
						const QString& middle = QString(),
						const QString& prefix = QString(),
						const QString& suffix = QString());
	void setName(const Name &name) {m_name = name;}
	/**
   * Returns a full name.
   * @return A full name.
   */
	const Name& name() const {return m_name;}
	/**
	* Sets a nickname.
	* @param nickname The nickname.
	*/
	void setNickname(const QString& nickname) {m_nickname = nickname;}
	/**
	* Returns the nickname.
	* @return The nickname.
	*/
	const QString& nickname() const {return m_nickname;}
	/**
	* Sets the birthday.
	* @param bday The birthday
	*/
	void setBday(const QDateTime& bday) {m_bday = bday;}
	/**
	* Returns the birthday.
	* @return The birthday.
	*/
	const QDateTime& bday() const {return m_bday;}
	/**
	* Sets a URI to a photo.
	* @param extval The URI to the photo.
	*/
	void setPhoto(const QString& extval);
	/**
	* Sets the photo directly.
	* @param type Format type parameter.
	* @param binval The binary photo data.
	* @note To remove the photo from the VCard call this function without any parameters.
	*/
	void setPhoto(const Photo &photo);
	/**
	* Returns photo information.
	* @return Info about the photo.
	*/
	const Photo& photo() const {return m_photo;}
	/**
	* Sets a URI to a organization logo.
	* @param extval The URI to the logo.
	*/
	//I want to be able to do so
	//Nickname *nickname() const {return findExtension<Nickname>().data();}
	//Photo *photo() const {return findExtension<Photo>().data();}
private:
	QScopedPointer<VCardPrivate> d_ptr;
	QString m_formattedName;
	QString m_nickname;
	Classification m_classification;
	Name m_name;
	QDateTime m_bday;
	Photo m_photo;
};

} // namespace jreen

Q_DECLARE_OPERATORS_FOR_FLAGS(jreen::VCard::Telephone::Types)

#endif // VCARD_H
