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

namespace Jreen
{

class VCardUpdatePrivate;

//XEP-0153: vCard-Based Avatars
//http://xmpp.org/extensions/xep-0153.html
class JREEN_EXPORT VCardUpdate : public Payload
{
	J_PAYLOAD(Jreen::VCardUpdate)
	Q_DECLARE_PRIVATE(VCardUpdate)
public:
	VCardUpdate(const QString &photoHash);
	VCardUpdate();
	~VCardUpdate();

	QString photoHash() const;
	void setPhotoHash(const QString &photoHash);
	void clear();
	bool hasPhotoInfo() const;

private:
	QScopedPointer<VCardUpdatePrivate> d_ptr;
};

} // namespace Jreen

#endif // VCARDUPDATE_H
