/****************************************************************************
 *  receipt.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef RECEIPT_H
#define RECEIPT_H

#include "stanzaextension.h"

namespace Jreen
{

class JREEN_EXPORT Receipt : public StanzaExtension
{
	J_EXTENSION(Jreen::Receipt,
		   "/message/request[@xmlns='urn:xmpp:receipts']"
		   "|/message/received[@xmlns='urn:xmpp:receipts']")
public:
	enum Type
	{
		Request		= 0,
		Received	= 1,
		Invalid		= -1
	};
	Receipt(Type type,const QString &id = QString());
	inline Type type() { return m_type; }
	inline QString id() {return m_id;}
	virtual ~Receipt() {}
private:
	Type m_type;
	QString m_id;
};

}

J_DECLARE_EXTENSION(Jreen::Receipt)

#endif // RECEIPT_H
