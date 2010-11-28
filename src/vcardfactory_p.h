/****************************************************************************
 *  vcardfactory_p.h
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

#ifndef VCARDFACTORY_P_H
#define VCARDFACTORY_P_H
#include "vcard.h"
#include "stanzaextension.h"

namespace jreen {

class VCardFactory : public StanzaExtensionFactory<VCard>
{
public:
    VCardFactory();
    virtual ~VCardFactory();
	QStringList features() const;
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	void serialize(StanzaExtension *extension, QXmlStreamWriter *writer);
	StanzaExtension::Ptr createExtension();	
private:
	void clear();
	enum State {//it make me cry
		AtFN,
		AtName,
		AtNameFamily,
		AtNameGiven,
		AtNameMiddle,
		AtNamePrefix,
		AtNameSuffix,
		AtBDay,
		AtNickname,
		AtPhoto,
		AtPhotoExtval,
		AtPhotoBinval,
		AtPhotoType
	};
	int m_depth;
	State m_state;
	QString m_fn;
	VCard::Classification m_classification;
	VCard::Name m_name;
	QString m_nickname;
	QString m_bday;
	VCard::Photo m_photo;
};

} // namespace jreen

#endif // VCARDFACTORY_P_H
