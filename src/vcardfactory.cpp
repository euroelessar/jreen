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

#include "vcardfactory_p.h"
#include "vcard_p.h"
#include <QXmlStreamWriter>
#include <QStringList>
#include "util.h"
#include "jstrings.h"
#define NS_VCARD QLatin1String("vcard-temp")

namespace Jreen {
	
AbstractStructureParser::AbstractStructureParser(const QLatin1String &name) : m_depth(0), m_name(name)
{
}
	
bool AbstractStructureParser::canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	return name == m_name;
}

void AbstractStructureParser::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	Q_UNUSED(uri);
	Q_UNUSED(attributes);
	m_depth++;
	m_currentString = 0;
	m_currentArray = 0;
	if (m_depth == 1) {
		for (int i = 0; i < m_strings.size(); i++)
			m_strings.at(i).second->clear();
		for (int i = 0; i < m_byteArrays.size(); i++)
			m_byteArrays.at(i).second->clear();
		for (int i = 0; i < m_flags.size(); i++)
			*m_flags.at(i).value = 0;
	} else if (m_depth == 2){
		for (int i = 0; i < m_strings.size(); i++) {
			const QPair<QLatin1String, QString*> &p = m_strings.at(i);
			if (p.first == name) {
				m_currentString = p.second;
				return;
			}
		}
		for (int i = 0; i < m_byteArrays.size(); i++) {
			const QPair<QLatin1String, QByteArray*> &p = m_byteArrays.at(i);
			if (p.first == name) {
				m_currentArray = p.second;
				return;
			}
		}
		for (int i = 0; i < m_flags.size(); i++) {
			const FlagInfo &info = m_flags.at(i);
			for (int j = 0; j < info.tableSize; j++) {
				if (name == QLatin1String(info.table[j])) {
					*info.value |= 1 << j;
					return;
				}
			}
		}
	}
}

void AbstractStructureParser::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth--;
	m_currentString = 0;
	m_currentArray = 0;
}

void AbstractStructureParser::handleCharacterData(const QStringRef &text)
{
	if (m_currentString) {
		*m_currentString = text.toString();
		m_currentString = 0;
	} else if (m_currentArray) {
		const QString str = QString::fromRawData(text.unicode(), text.size());
		*m_currentArray = QByteArray::fromBase64(str.toLatin1());
		m_currentArray = 0;
	}
}

void AbstractStructureParser::addByteArray(const QLatin1String &name, QByteArray *str)
{
	m_byteArrays.append(qMakePair(name, str));
}

void AbstractStructureParser::addString(const QLatin1String &name, QString *str)
{
	m_strings.append(qMakePair(name, str));
}

void AbstractStructureParser::addFlag(const char **table, int size, int *value)
{
	m_flags.append(FlagInfo());
	FlagInfo &info = m_flags.last();
	info.table = table;
	info.tableSize = size;
	info.value = value;
}

template <typename T, typename L>
T *getValue(void *zeroc, L zeroData, void *data)
{
	int offset = reinterpret_cast<char*>(zeroData) - reinterpret_cast<char*>(zeroc);
	return reinterpret_cast<T*>(reinterpret_cast<char*>(data) + offset);
}

void AbstractStructureParser::serialize(void *zero, void *data, QXmlStreamWriter *writer)
{
	bool hasAnyChild = false;
	// I don't like this copy&paste, but don't know good way to avoid it right now
	for (int i = 0; !hasAnyChild && i < m_strings.size(); i++) {
		QString *str = getValue<QString>(zero, m_strings.at(i).second, data);
		hasAnyChild = !str->isEmpty();
	}
	for (int i = 0; !hasAnyChild && i < m_byteArrays.size(); i++) {
		QByteArray *str = getValue<QByteArray>(zero, m_byteArrays.at(i).second, data);
		hasAnyChild = !str->isEmpty();
	}
	for (int i = 0; !hasAnyChild && i < m_flags.size(); i++)
		hasAnyChild = *m_flags.at(i).value;
	if (!hasAnyChild)
		return;
	writer->writeStartElement(m_name);
	for (int i = 0; i < m_strings.size(); i++) {
		QString *str = getValue<QString>(zero, m_strings.at(i).second, data);
		if (!str->isEmpty())
			writer->writeTextElement(m_strings.at(i).first, *str);
	}
	for (int i = 0; i < m_byteArrays.size(); i++) {
		QByteArray *str = getValue<QByteArray>(zero, m_byteArrays.at(i).second, data);
		if (!str->isEmpty()) {
			QByteArray data = str->toBase64();
			QString encoded = QString::fromLatin1(data, data.size());
			writer->writeTextElement(m_byteArrays.at(i).first, encoded);
		}
	}
	for (int i = 0; i < m_flags.size(); i++) {
		const FlagInfo &info = m_flags.at(i);
		int *value = getValue<int>(zero, info.value, data);
		for (int j = 0; j < info.tableSize; j++) {
			if ((*value) & (1 << j))
				writer->writeEmptyElement(QLatin1String(info.table[j]));
		}
	}
	writer->writeEndElement();
}

static const char* vcardNameTypes[] = {
	"FAMILY",
	"GIVEN",
	"MIDDLE",
	"PREFIX",
	"SUFFIX"
};

class VCardNameParser : public StructurePrivateParser<VCard::NamePrivate, VCard::Name>
{
public:
	VCardNameParser() : StructurePrivateParser<VCard::NamePrivate, VCard::Name>(QLatin1String("N"))
	{
		QString *strings[] = {
			&m_data.family, &m_data.given, &m_data.middle,
			&m_data.prefix, &m_data.suffix
		};
		for (int i = 0, size = sizeof(strings)/sizeof(QString*); i < size; i++)
			addString(QLatin1String(vcardNameTypes[i]), strings[i]);
	}
};

static const char* vcardPhotoTypes[] = {
	"BINVAL",
	"EXTVAL",
	"TYPE"
};

class VCardPhotoParser : public StructurePrivateParser<VCard::PhotoPrivate, VCard::Photo>
{
public:
	VCardPhotoParser() : StructurePrivateParser<VCard::PhotoPrivate, VCard::Photo>(QLatin1String("PHOTO"))
	{
		QString *strings[] = {
			&m_data.extval, &m_data.type
		};
		for (int i = 1, size = sizeof(strings)/sizeof(QString*); i < size; i++)
			addString(QLatin1String(vcardPhotoTypes[i]), strings[i]);
		addByteArray(QLatin1String(vcardPhotoTypes[0]), &m_data.binval);
	}
};

static const char* vcardTelTypes[] = {
	"HOME",
    "WORK",
    "VOICE",
    "FAX",
    "PAGER",
    "MSG",
    "CELL",
    "VIDEO",
    "BBS",
    "MODEM",
    "ISDN",
    "PCS",
    "PREF"
};

class VCardTelParser : public StructurePrivateParser<VCard::TelephonePrivate, VCard::Telephone>
{
public:
	VCardTelParser() : StructurePrivateParser<VCard::TelephonePrivate, VCard::Telephone>(QLatin1String("TEL"))
	{
		addString(QLatin1String("NUMBER"), &m_data.number);
		addFlag(vcardTelTypes, &m_data.types);
	}
};

static const char* vcardEMailTypes[] = {
	"HOME",
	"WORK",
	"INTERNET",
	"PREF",
	"X400"
};

class VCardEMailParser : public StructurePrivateParser<VCard::EMailPrivate, VCard::EMail>
{
public:
	VCardEMailParser() : StructurePrivateParser<VCard::EMailPrivate, VCard::EMail>(QLatin1String("EMAIL"))
	{
		addString(QLatin1String("USERID"), &m_data.userId);
		addFlag(vcardEMailTypes, &m_data.types);
	}
};

static const char* vcardAddressTypes[] = {
	"HOME",
	"WORK",
	"POSTAL",
	"PARCEL",
	"DOM",
	"INTL",
	"PREF"
};

static const char* vcardAddressFields[] = {
	"POBOX",
	"EXTADD",
	"STREET",
	"LOCALITY",
	"REGION",
	"PCODE",
	"CTRY"
};

class VCardAddressParser : public StructurePrivateParser<VCard::AddressPrivate, VCard::Address>
{
public:
	VCardAddressParser() : StructurePrivateParser<VCard::AddressPrivate, VCard::Address>(QLatin1String("ADR"))
	{
		QString *strings[] = {
			&m_data.pobox, &m_data.extendedAddress, &m_data.street,
			&m_data.locality, &m_data.region, &m_data.pcode,
			&m_data.country
		};
		for (int i = 0, size = sizeof(strings)/sizeof(QString*); i < size; i++)
			addString(QLatin1String(vcardAddressFields[i]), strings[i]);
		addFlag(vcardAddressTypes, &m_data.types);
	}
};

class VCardOrgParser : public StructurePrivateParser<VCard::OrganizationPrivate, VCard::Organization>
{
public:
	VCardOrgParser();
	void serialize(const VCard::Organization &org, QXmlStreamWriter *writer);
protected:
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);

private:
	bool atOrgUnit;
};

VCardOrgParser::VCardOrgParser() :
	StructurePrivateParser<VCard::OrganizationPrivate, VCard::Organization>(QLatin1String("ORG")),
	atOrgUnit(false)
{
	addString(QLatin1String("ORGNAME"), &m_data.orgName);
}

void VCardOrgParser::serialize(const VCard::Organization &org, QXmlStreamWriter *writer)
{
	QString name = org.name();
	QStringList units = org.units();
	if (name.isEmpty() && units.isEmpty())
		return;

	writer->writeStartElement(QLatin1String("ORG"));
	if (!name.isEmpty())
		writer->writeTextElement(QLatin1String("ORGNAME"), name);
	foreach (const QString &unit, org.units())
		writer->writeTextElement(QLatin1String("ORGUNIT"), unit);
	writer->writeEndElement();
}

void VCardOrgParser::handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes)
{
	AbstractStructureParser::handleStartElement(name, uri, attributes);
	if (m_depth == 1) {
		m_data.orgUnits.clear();
	} else if (m_depth == 2) {
		if (name == QLatin1String("ORGUNIT"))
			atOrgUnit = true;
	}
}

void VCardOrgParser::handleEndElement(const QStringRef &name, const QStringRef &uri)
{
	if (m_depth == 2 && name == QLatin1String("ORGUNIT"))
		atOrgUnit = false;
	AbstractStructureParser::handleEndElement(name, uri);
}

void VCardOrgParser::handleCharacterData(const QStringRef &text)
{
	if (atOrgUnit)
		m_data.orgUnits << text.toString();
	else
		AbstractStructureParser::handleCharacterData(text);
}

static const char* vcardTypes[] = {
	"FN",
	"BDAY",
	"NICKNAME",
    "URL",
    "JABBERID",
    "TITLE",
    "ROLE",
    "NOTE",
    "DESC",
    "MAILER",
    "TZ",
    "PRODID",
    "REV",
    "SORT-STRING",
    "UID"
};

enum VCardType
{
	FormattedName,
	Birthday, // QDateTime
	Nickname,
	Url, // QUrl
	JabberID, // JID
	Title,
	Role,
	Note,
	Description,
	Mailer,
	TimeZone,
	ProductID,
	Revision,
	SortString,
	UserID,
	LastVCardType
};

void vCardStringHelper(QString * & str, VCardPrivate *p, int type)
{
	QString *strings[] = {
		&p->formattedName, 0, &p->nickname, 0, 0, &p->title,
		&p->role, &p->note, &p->description, &p->mailer, &p->timeZone,
		&p->productID, &p->revision, &p->sortString, &p->userID
	};
	if (strings[type])
		str = strings[type];
}

class JREEN_AUTOTEST_EXPORT VCardFactoryPrivate
{
public:
	void clear();
	
	int depth;
	VCardFactory::State state;
	
	QScopedPointer<VCardPrivate> vcard;
	VCardNameParser nameParser;
	VCardPhotoParser photoParser;
	VCardTelParser telParser;
	VCardEMailParser emailParser;
	VCardAddressParser addressParser;
	VCardOrgParser orgParser;
	
	XmlStreamParser *currentParser;
	
	VCard::Classification classification;
	QString *currentString;
	QString tmpString;
};

VCardFactory::VCardFactory() : d_ptr(new VCardFactoryPrivate)
{
	Q_D(VCardFactory);
	d->currentParser = 0;
	d->depth = 0;
	d->classification = VCard::ClassNone;
}


bool VCardFactory::canParse(const QStringRef& name, const QStringRef& uri, const QXmlStreamAttributes& attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("vCard") && uri == NS_VCARD;
}

Payload::Ptr VCardFactory::createPayload()
{
	Q_D(VCardFactory);
	return Payload::Ptr(new VCard(*d->vcard.take()));
}

QStringList VCardFactory::features() const
{
	return QStringList(NS_VCARD);
}

void VCardFactory::handleStartElement(const QStringRef& name, const QStringRef& uri,
									  const QXmlStreamAttributes& attributes)
{
	Q_D(VCardFactory);
	d->depth++;
	if (d->depth == 1) {
		d->vcard.reset(new VCardPrivate);
		d->state = AtRoot;
	} else if (d->depth == 2) {
		if (d->nameParser.canParse(name, uri, attributes)) {
			d->currentParser = &d->nameParser;
			d->state = AtName;
		} else if (d->photoParser.canParse(name, uri, attributes)) {
			d->currentParser = &d->photoParser;
			d->state = AtPhoto;
		} else if (d->telParser.canParse(name, uri, attributes)) {
			d->currentParser = &d->telParser;
			d->state = AtTelephone;
		} else if (d->emailParser.canParse(name, uri, attributes)) {
			d->currentParser = &d->emailParser;
			d->state = AtEMail;
		} else if (d->addressParser.canParse(name, uri, attributes)) {
			d->currentParser = &d->addressParser;
			d->state = AtAddress;
		} else if (d->orgParser.canParse(name, uri, attributes)) {
			d->currentParser = &d->orgParser;
			d->state = AtOrg;
		} else {
			int index = strToEnum(name, vcardTypes);
			if (index == -1) {
				d->state = AtNowhere;
				d->currentString = NULL;
			} else {
				d->state = static_cast<State>(LastState + index);
				d->currentString = &d->tmpString;
				d->tmpString.clear();
				vCardStringHelper(d->currentString, d->vcard.data(), index);
			}
		}
	}
	if (d->currentParser)
		d->currentParser->handleStartElement(name, uri, attributes);
}

void VCardFactory::handleCharacterData(const QStringRef& text)
{
	Q_D(VCardFactory);
	if (d->currentParser)
		d->currentParser->handleCharacterData(text); 
	else if (d->depth == 2 && d->currentString)
		*d->currentString = text.toString();
}

void VCardFactory::handleEndElement(const QStringRef& name, const QStringRef& uri)
{
	Q_D(VCardFactory);
	Q_UNUSED(name);
	Q_UNUSED(uri);
	if (d->currentParser)
		d->currentParser->handleEndElement(name, uri);
	if (d->depth == 2 && d->currentParser) {
		if (d->state == AtName)
			d->vcard->name = d->nameParser.create();
		else if (d->state == AtPhoto)
			d->vcard->photo = d->photoParser.create();
		else if (d->state == AtTelephone)
			d->vcard->telephones << d->telParser.create();
		else if (d->state == AtEMail)
			d->vcard->emails << d->emailParser.create();
		else if (d->state == AtAddress)
			d->vcard->addresses << d->addressParser.create();
		else if (d->state == AtOrg)
			d->vcard->org = d->orgParser.create();
		d->state = AtRoot;
		d->currentParser = 0;
	} else if (d->depth == 2 && d->state != AtNowhere && d->currentString) {
		if (d->currentString == &d->tmpString) {
			int index = d->state - LastState;
			if (index == FormattedName)
				d->vcard->formattedName = d->tmpString;
			else if (index == Birthday)
				d->vcard->bday = Util::fromStamp(d->tmpString);
			else if (index == Url)
				d->vcard->url = QUrl::fromUserInput(d->tmpString);
			else if (index == JabberID)
				d->vcard->jabberId = d->tmpString;
		}
		d->state = AtRoot;
		d->currentString = 0;
	} else if (d->depth == 2) {
		d->state = AtRoot;
	}
	d->depth--;
}

inline void serializeHelper(int enumValue, const QString &value, QXmlStreamWriter* writer)
{
	if (!value.isEmpty())
		writer->writeTextElement(enumToStr(enumValue, vcardTypes), value);
}

void VCardFactory::serialize(Payload* extension, QXmlStreamWriter* writer)
{
	Q_D(VCardFactory);
	VCardPrivate *vcard = VCardPrivate::get(se_cast<VCard*>(extension));

	writer->writeStartElement(QLatin1String("vCard"));
	writer->writeDefaultNamespace(NS_VCARD);
	QString tmp;
	for (int i = 0; i < LastVCardType; i++) {
		QString *current = &tmp;
		if (i == FormattedName) {
			tmp = vcard->formattedName;
		} else if (i == Birthday) {
			if (!vcard->bday.isValid())
				continue;
			else if (vcard->bday.time().isNull())
				tmp = Util::toStamp(vcard->bday.date());
			else
				tmp = Util::toStamp(vcard->bday);
		} else if (i == Url) {
			tmp = QString::fromUtf8(vcard->url.toEncoded());
		} else if (i == JabberID) {
			tmp = vcard->jabberId;
		} else {
			vCardStringHelper(current, vcard, i);
		}
		if (!current->isEmpty())
			serializeHelper(i, *current, writer);
	}
	d->nameParser.serialize(vcard->name, writer);
	d->photoParser.serialize(vcard->photo, writer);
	for (int i = 0; i < vcard->telephones.size(); i++)
		d->telParser.serialize(vcard->telephones.at(i), writer);
	for (int i = 0; i < vcard->emails.size(); i++)
		d->emailParser.serialize(vcard->emails.at(i), writer);
	for (int i = 0; i < vcard->addresses.size(); i++)
		d->addressParser.serialize(vcard->addresses.at(i), writer);
	d->orgParser.serialize(vcard->org, writer);
	writer->writeEndElement();
}

VCardFactory::~VCardFactory()
{

}

} // namespace Jreen
