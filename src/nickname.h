/****************************************************************************
 *  nickname.h
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

#ifndef NICKNAME_H
#define NICKNAME_H

#include "stanzaextension.h"

namespace Jreen
{

class NicknamePrivate;
class Nickname : public Payload
{
	Q_DECLARE_PRIVATE(Nickname)
	J_PAYLOAD(Jreen::Nickname)
public:
	Nickname(const QString &nick);
	~Nickname();
	
	QString nick() const;
private:
	QScopedPointer<NicknamePrivate> d_ptr;
};

}

#endif // NICKNAME_H
