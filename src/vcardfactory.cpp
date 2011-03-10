/****************************************************************************
 *  vcardfactory.cpp
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
}

void AbstractStructureParser::handleCharacterData(const QStringRef &text)
{
	if (m_currentString) {
		*m_currentString = text.toString();
	} else if (m_currentArray) {
		const QString str = QString::fromRawData(text.unicode(), text.size());
		*m_currentArray = QByteArray::fromBase64(str.toLatin1());
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

void AbstractStructureParser::serialize(void *zero, void *data, QXmlStreamWriter *writer)
{
	char *zeroc = reinterpret_cast<char*>(zero);
	bool hasAnyChild = false;
	// I don't like this copy&paste, but don't know good way to avoid it right now
	for (int i = 0; !hasAnyChild && i < m_strings.size(); i++) {
		int offset = reinterpret_cast<char*>(m_strings.at(i).second) - zeroc;
		QString *str = reinterpret_cast<QString*>(reinterpret_cast<char*>(data) + offset);
		hasAnyChild = !str->isEmpty();
	}
	for (int i = 0; !hasAnyChild && i < m_byteArrays.size(); i++) {
		int offset = reinterpret_cast<char*>(m_byteArrays.at(i).second) - zeroc;
		QByteArray *str = reinterpret_cast<QByteArray*>(reinterpret_cast<char*>(data) + offset);
		hasAnyChild = !str->isEmpty();
	}
	for (int i = 0; !hasAnyChild && i < m_flags.size(); i++)
		hasAnyChild = *m_flags.at(i).value;
	if (!hasAnyChild)
		return;
	writer->writeStartElement(m_name);
	for (int i = 0; i < m_strings.size(); i++) {
		int offset = reinterpret_cast<char*>(m_strings.at(i).second) - zeroc;
		QString *str = reinterpret_cast<QString*>(reinterpret_cast<char*>(data) + offset);
		if (!str->isEmpty())
			writer->writeTextElement(m_strings.at(i).first, *str);
	}
	for (int i = 0; !hasAnyChild && i < m_byteArrays.size(); i++) {
		int offset = reinterpret_cast<char*>(m_byteArrays.at(i).second) - zeroc;
		QByteArray *str = reinterpret_cast<QByteArray*>(reinterpret_cast<char*>(data) + offset);
		if (!str->isEmpty()) {
			QByteArray data = str->toBase64();
			QString encoded = QString::fromLatin1(data, data.size());
			writer->writeTextElement(m_byteArrays.at(i).first, encoded);
		}
	}
	for (int i = 0; i < m_flags.size(); i++) {
		const FlagInfo &info = m_flags.at(i);
		for (int j = 0; j < info.tableSize; j++) {
			if ((*info.value) & (1 << j))
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
	VCardNameParser() : StructurePrivateParser<VCard::NamePrivate, VCard::Name>(QLatin1String("NAME"))
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

static const char* vcardTypes[] = {
	"FN",
	"BDAY",
	"NICKNAME"
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

class VCardFactoryPrivate
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
	VCardAddressParser vcardParser;
	
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

StanzaExtension::Ptr VCardFactory::createExtension()
{
	Q_D(VCardFactory);
	return StanzaExtension::Ptr(d->vcard ? new VCard(*d->vcard.take()) : 0); //here is segfault
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
		} else {
			int index = strToEnum(name, vcardTypes);
			d->state = static_cast<State>(LastState + index);
			d->currentString = &d->tmpString;
			d->tmpString.clear();
			if (index > -1)
				vCardStringHelper(d->currentString, d->vcard.data(), index);
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
		d->currentParser = 0;
	} else if (d->depth == 2 && d->currentString) {
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
		d->currentString = 0;
	}
	d->depth--;
}

inline void serializeHelper(int enumValue, const QString &value, QXmlStreamWriter* writer)
{
	if (!value.isEmpty())
		writer->writeTextElement(enumToStr(enumValue, vcardTypes), value);
}

void VCardFactory::serialize(StanzaExtension* extension, QXmlStreamWriter* writer)
{
	Q_D(VCardFactory);
	VCardPrivate *vcard = VCardPrivate::get(se_cast<VCard*>(extension));

	writer->writeStartElement(QLatin1String("vCard"));
	writer->writeDefaultNamespace(NS_VCARD);
	QString tmp;
	for (int i = 0; i < LastVCardType; i++) {
		QString *current = &tmp;
		if (i == FormattedName)
			tmp = vcard->formattedName;
		else if (i == Birthday)
			tmp = Util::toStamp(vcard->bday);
		else if (i == Url)
			tmp = QString::fromUtf8(vcard->url.toEncoded());
		else if (i == JabberID)
			tmp = vcard->jabberId;
		else
			vCardStringHelper(current, vcard, i);
		if (!current->isEmpty())
			serializeHelper(i, *current, writer);
	}
	d->nameParser.serialize(vcard->name, writer);
	d->photoParser.serialize(vcard->photo, writer);
	for (int i = 0; i < vcard->telephones.size(); i++)
		d->telParser.serialize(vcard->telephones.at(i), writer);
	for (int i = 0; i < vcard->emails.size(); i++)
		d->emailParser.serialize(vcard->emails.at(i), writer);
	writer->writeEndElement();
}

VCardFactory::~VCardFactory()
{

}

} // namespace Jreen
