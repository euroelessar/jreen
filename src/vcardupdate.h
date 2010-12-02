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

namespace jreen {

//XEP-0153: vCard-Based Avatars
//http://xmpp.org/extensions/xep-0153.html
class JREEN_EXPORT VCardUpdate : public StanzaExtension
{
	J_EXTENSION(jreen::VCardUpdate,
				"/presence/vcard[@xmlns='vcard-temp:x:update']")
public:
	VCardUpdate(const QString &photoHash) : m_photoHash(photoHash) {};
	const QString &photoHash() const { return m_photoHash;}
private:
	QString m_photoHash;
};

} // namespace jreen

#endif // VCARDUPDATE_H
