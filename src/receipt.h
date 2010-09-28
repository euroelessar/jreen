/****************************************************************************
 *  receipt.h
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

#ifndef RECEIPT_H
#define RECEIPT_H

#include "stanzaextension.h"

namespace jreen
{

class Receipt : public StanzaExtension
{
	J_EXTENSION(Receipt,
		   "/message/request[@xmlns='urn:xmpp:receipts']"
		   "|/message/received[@xmlns='urn:xmpp:receipts']")
public:
	enum Type
	{
		Request,
		Received,
		Invalid
	};
	Receipt(const QDomElement &node = QDomElement());
	Receipt(Type type);
	inline Type type() { return m_type; }
	QDomElement node(QDomDocument *doc) const;
private:
	Type m_type;
};

}

#endif // RECEIPT_H
