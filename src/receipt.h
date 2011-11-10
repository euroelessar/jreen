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

class ReceiptPrivate;
class JREEN_EXPORT Receipt : public Payload
{
	Q_DECLARE_PRIVATE(Receipt)
	J_PAYLOAD(Jreen::Receipt)
public:
	enum Type
	{
		Request		= 0,
		Received	= 1,
		Invalid		= -1
	};

	Receipt(Type type,const QString &id = QString());
	~Receipt();

	Type type() const;
	QString id() const;
private:
	QScopedPointer<ReceiptPrivate> d_ptr;
};

}

#endif // RECEIPT_H
