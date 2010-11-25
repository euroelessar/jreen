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

//implementation of XEP 0054
//http://xmpp.org/extensions/xep-0054.html
namespace jreen {

class VCard : public StanzaExtension
{
	J_EXTENSION(jreen::VCard,
		   "/iq/vcard[@xmlns='vcard-temp']")
public:
    VCard();
};

} // namespace jreen

#endif // VCARD_H
