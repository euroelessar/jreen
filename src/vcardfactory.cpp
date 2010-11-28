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
#include <QXmlStreamWriter>
#include <QStringList>
#include "util.h"
#include "jstrings.h"
#define NS_VCARD QLatin1String("vcard-temp")

namespace jreen {

static const char* vcard_types[] = {
	"FN",
	"NAME",
	"FAMILY",
	"GIVEN",
	"MIDDLE",
	"PREFIX",
	"SUFFIX",
	"BDAY",
	"NICKNAME",
	"PHOTO",
	"EXTVAL",
	"BINVAL",
	"TYPE"
};

VCardFactory::VCardFactory()
{
	m_fn.clear();
	m_classification = VCard::ClassNone;
	m_name = VCard::Name();
	m_nickname.clear();
	m_bday.clear();
}


bool VCardFactory::canParse(const QStringRef& name, const QStringRef& uri, const QXmlStreamAttributes& attributes)
{
	Q_UNUSED(attributes);
	return name == QLatin1String("vCard") && uri == NS_VCARD;
}

StanzaExtension::Ptr VCardFactory::createExtension()
{
	VCard *vcard = new VCard(m_fn,m_classification);
	vcard->setName(m_name);
	vcard->setNickname(m_nickname);
	vcard->setBday(Util::fromStamp(m_bday));
	vcard->setPhoto(m_photo);
	return StanzaExtension::Ptr(vcard);
}

QStringList VCardFactory::features() const
{
	return QStringList(NS_VCARD);
}

void VCardFactory::handleStartElement(const QStringRef& name, const QStringRef& uri,
									  const QXmlStreamAttributes& attributes)
{
	Q_UNUSED(attributes);
	Q_UNUSED(uri);
	m_depth++;
	if(m_depth == 1) {

	} else {
		//black magic
		m_state = strToEnum<State>(name,vcard_types);
	}
}

void VCardFactory::handleCharacterData(const QStringRef& text)
{
	switch(m_state) {
	case AtFN:
		m_fn = text.toString();
		break;
	case AtName:
		m_name = VCard::Name(); //clear
		break;
	case AtNameFamily:
		m_name.family = text.toString();
		break;
	case AtNameGiven:
		m_name.given = text.toString();
		break;
	case AtNameMiddle:
		m_name.middle = text.toString();
		break;
	case AtNamePrefix:
		m_name.prefix = text.toString();
		break;
	case AtNameSuffix:
		m_name.suffix = text.toString();
		break;
	case AtBDay:
		m_bday = text.toString();
		break;
	case AtNickname:
		m_nickname = text.toString();
		break;
	case AtPhoto:
		m_photo = VCard::Photo(); //clear
		break;
	case AtPhotoExtval:
		m_photo.extval = text.toString();
		break;
	case AtPhotoBinval:
		m_photo.binval = text.toString();
		break;
	case AtPhotoType:
		m_photo.type = text.toString();
		break;
	}
}

void VCardFactory::handleEndElement(const QStringRef& name, const QStringRef& uri)
{
	Q_UNUSED(name);
	Q_UNUSED(uri);
	m_depth--;
}

void VCardFactory::serialize(StanzaExtension* extension, QXmlStreamWriter* writer)
{
	VCard *vcard = se_cast<VCard*>(extension);

	writer->writeStartElement(QLatin1String("vCard"));
	writer->writeDefaultNamespace(NS_VCARD);
	writer->writeTextElement(enumToStr(AtFN,vcard_types),vcard->formattedName());
	//write name
	writer->writeStartElement(enumToStr(AtName,vcard_types));
	writer->writeTextElement(enumToStr(AtNameFamily,vcard_types),vcard->name().family);
	writer->writeTextElement(enumToStr(AtNameGiven,vcard_types),vcard->name().given);
	writer->writeTextElement(enumToStr(AtNameMiddle,vcard_types),vcard->name().middle);
	writer->writeTextElement(enumToStr(AtNamePrefix,vcard_types),vcard->name().prefix);
	writer->writeTextElement(enumToStr(AtNameSuffix,vcard_types),vcard->name().suffix);
	writer->writeEndElement();
	
	writer->writeTextElement(enumToStr(AtNickname,vcard_types),vcard->nickname());
	writer->writeTextElement(enumToStr(AtBDay,vcard_types),Util::toStamp(vcard->bday()));
	//write photo
	writer->writeStartElement(enumToStr(AtPhoto,vcard_types));
	writer->writeTextElement(enumToStr(AtPhotoExtval,vcard_types),vcard->photo().extval);
	writer->writeTextElement(enumToStr(AtPhotoBinval,vcard_types),vcard->photo().binval);
	writer->writeTextElement(enumToStr(AtPhotoType,vcard_types),vcard->photo().type);
	writer->writeEndElement();
}

VCardFactory::~VCardFactory()
{

}

} // namespace jreen
