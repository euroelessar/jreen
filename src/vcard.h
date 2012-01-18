/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef VCARD_H
#define VCARD_H
#include "stanzaextension.h"
#include "nickname.h"
#include <QDateTime>

//implementation of XEP 0054
//http://xmpp.org/extensions/xep-0054.html
namespace Jreen {

class VCardPrivate;

class JREEN_EXPORT VCard : public Payload
{
	J_PAYLOAD(Jreen::VCard)
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

	class NamePrivate;
	class PhotoPrivate;
	class AddressPrivate;
	class TelephonePrivate;
	class EMailPrivate;
	class OrganizationPrivate;

	class JREEN_EXPORT Name
	{
	public:
		Name();
		Name(const Name &o);
		Name(NamePrivate &p);
		~Name();
		Name &operator =(const Name &o);
		
		QString family() const;
		void setFamily(const QString &);
		QString given() const;
		void setGiven(const QString &);
		QString middle() const;
		void setMiddle(const QString &);
		QString prefix() const;
		void setPrefix(const QString &);
		QString suffix() const;
		void setSuffix(const QString &);
	private:
		QSharedDataPointer<NamePrivate> d_ptr;
		friend class NamePrivate;
	};

	class JREEN_EXPORT Photo
	{
	public:
		Photo();
		Photo(const Photo &o);
		Photo(PhotoPrivate &p);
		~Photo();
		Photo &operator =(const Photo &o);
		
		QString external() const;
		void setExternal(const QString &);
		void setData(const QByteArray &data, const QString &mimeType = QString());
		QByteArray data() const;
		QString mimeType() const;
	private:
		QSharedDataPointer<PhotoPrivate> d_ptr;
		friend class PhotoPrivate;
	};
	
	class JREEN_EXPORT Telephone
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
		
		Telephone();
		Telephone(const Telephone &o);
		Telephone(TelephonePrivate &p);
		~Telephone();
		Telephone &operator =(const Telephone &o);

		bool testType(Type t) const;
		void setType(Type t, bool value);
		QString number() const;
		void setNumber(const QString &);
	private:
		QSharedDataPointer<TelephonePrivate> d_ptr;
		friend class TelephonePrivate;
	};

	class JREEN_EXPORT EMail
	{
	public:
		enum Type
		{
			Home      = 0x01,
			Work      = 0x02,
			Internet  = 0x04,
			Preferred = 0x08,
			X400      = 0x10
		};
		EMail();
		EMail(const EMail &o);
		EMail(EMailPrivate &p);
		~EMail();
		EMail &operator =(const EMail &o);
		bool testType(Type t) const;
		const QString &userId() const;
		void setUserId(const QString &userId);
		void setType(Type t, bool value);
	private:
		QSharedDataPointer<EMailPrivate> d_ptr;
		friend class EMailPrivate;
	};

	class JREEN_EXPORT Address
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
		Address();
		Address(const Address &o);
		Address(AddressPrivate &p);
		~Address();
		Address &operator =(const Address &o);
		bool testType(Type t) const;
		QString postBox() const;
		QString extendedAddress() const;
		QString street() const;
		QString locality() const;
		QString region() const;
		QString postCode() const;
		QString country() const;
		void setType(Type t, bool value);
		void setPostBox(const QString &postBox);
		void setExtendedAddress(const QString &extendedAddress);
		void setStreet(const QString &street);
		void setLocality(const QString &locality);
		void setRegion(const QString &region);
		void setPostCode(const QString &postCode);
		void setCountry(const QString &country);
	private:
		QSharedDataPointer<AddressPrivate> d_ptr;
		friend class AddressPrivate;
	};

	class JREEN_EXPORT Organization
	{
	public:
		Organization();
		Organization(const Organization &o);
		Organization(OrganizationPrivate &p);
		~Organization();
		Organization &operator =(const Organization &o);
		QString name() const;
		QStringList units() const;
		void setName(const QString &name);
		void setUnits(const QStringList &units);
		void addUnit(const QString &unit);
	private:
		QSharedDataPointer<OrganizationPrivate> d_ptr;
		friend class OrganizationPrivate;
	};

	VCard(const QString &formattedName = QString(), Classification classification = ClassNone);
	VCard(VCardPrivate &p);
	~VCard();
	/**
	* Sets the formatted name.
	* @param name The formatted name.
	*/
	inline void setFormattedName(const QString &fn);
	/**
	* Returns the formatted name.
	* @return The formatted name.
	*/
	const QString &formattedName() const;
	void setName(const QString& family, const QString& given,
				 const QString& middle = QString(),
				 const QString& prefix = QString(),
				 const QString& suffix = QString());
	void setName(const Name &name);
	/**
	* Returns a full name.
	* @return A full name.
	*/
	const Name& name() const;
	/**
	* Sets a nickname.
	* @param nickname The nickname.
	*/
	void setNickname(const QString& nickname);
	/**
	* Returns the nickname.
	* @return The nickname.
	*/
	QString nickname() const;
	/**
	* Sets the birthday.
	* @param bday The birthday
	*/
	Q_DECL_DEPRECATED void setBday(const QDateTime &bday);
	void setBirthday(const QDateTime &bday);
	void setBirthday(const QDate &bday);
	/**
	* Returns the birthday.
	* @return The birthday.
	*/
	Q_DECL_DEPRECATED QDateTime bday() const;
	QDateTime birthday() const;
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
	Photo photo() const;
	/**
	* Set the url.
	* @param url The url.
	*/
	void setUrl(const QUrl& url);
	/**
	* Returns the url.
	* @return The url.
	*/
	const QUrl& url() const;
	/**
	* Returns a list of telephone numbers.
	* @return A list of telephone numbers.
	*/
	QList<VCard::Telephone> telephones() const;
	/**
	* Adds a telephone number.
	* @param number The telephone number.
	* @param type Bit-wise ORed @ref AddressType.
	*/
	void addTelephone(const Telephone &telephone);
	/**
	* Adds an email address.
	* @param userid The email address.
	* @param type Bit-wise ORed @ref AddressType.
	*/
	void addEmail(const EMail &email);
	/**
	* Returns a list of email addresses.
	* @return A list of email addresses.
	*/
	QList<VCard::EMail> emails() const;
	/**
	* Returns a list of addresses.
	* @return A list of addresses.
	*/
	QList<Address> addresses() const;
	/**
	* Adds an address.
	*/
	void addAdress(const Address &adr);
	/**
	* Sets a "free-form descriptive text".
	* @param desc The descriptive text.
	*/
	void setDesc(const QString& desc);
	/**
	* Returns the "free-form descriptive text".
	* @return The descriptive text.
	*/
	const QString& desc() const;
	/**
	* Sets the person's title.
	* @param title The title.
	*/
	void setTitle(const QString& title);
	/**
	* Returns the person's title.
	* @return The title.
	*/
	QString title() const;
	/**
	* Sets the person's role.
	* @param role The role.
	*/
	void setRole(const QString& role);
	/**
	* Returns the person's role.
	* @return The role.
	*/
	QString role() const;
	/**
	* Sets the organization.
	* @param org The organization.
	*/
	void setOrganization(const Organization &org);
	/**
	* Sets the organization.
	* @param orgName The organization's name.
	* @param orgUnits A list of organizational units.
	*/
	void setOrganization(const QString &orgName, const QStringList &orgUnits);
	/**
	* Returns the organization.
	* @return The organization.
	*/
	Organization organization();
private:
	QScopedPointer<VCardPrivate> d_ptr;
};

} // namespace Jreen

Q_DECLARE_OPERATORS_FOR_FLAGS(Jreen::VCard::Telephone::Types)

#endif // VCARD_H
