/****************************************************************************
 *  vcardupdate.h
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

#ifndef VCARDUPDATE_H
#define VCARDUPDATE_H
#include "stanzaextension.h"

namespace Jreen {

//XEP-0153: vCard-Based Avatars
//http://xmpp.org/extensions/xep-0153.html
class JREEN_EXPORT VCardUpdate : public StanzaExtension
{
	J_EXTENSION(Jreen::VCardUpdate,
				"/presence/vcard[@xmlns='vcard-temp:x:update']")
public:
	VCardUpdate(const QString &photoHash) : m_photoHash(photoHash), m_hasPhotoInfo(true) {}
	VCardUpdate() : m_hasPhotoInfo(false) {}
	const QString &photoHash() const { return m_photoHash;}
	void setPhotoHash(const QString &photoHash) { m_photoHash = photoHash; m_hasPhotoInfo = !photoHash.isNull(); }
	bool hasPhotoInfo() const { return m_hasPhotoInfo; }
private:
	QString m_photoHash;
	bool m_hasPhotoInfo;
};

} // namespace Jreen

#endif // VCARDUPDATE_H
