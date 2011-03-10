/****************************************************************************
 *  imagemessage.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef IMAGEMESSAGE_H
#define IMAGEMESSAGE_H

#include "stanzaextension.h"

namespace Jreen
{

class ImageMessage : public StanzaExtension
{
	J_EXTENSION(Jreen::ImageMessage,"/message/img[@xmlns='qutim:img']")
public:
	ImageMessage(const QDomElement &node = QDomElement());
	ImageMessage(const QByteArray &data);
	inline const QByteArray &data() { return m_data; }
	QDomElement node(QDomDocument *doc) const;
private:
	QByteArray m_data;
};

}

#endif // IMAGEMESSAGE_H
