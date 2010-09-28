/****************************************************************************
 *  domcreater_p.h
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

#ifndef DOMCREATER_P_H
#define DOMCREATER_P_H

#include "jreen.h"
#include <QDomElement>

namespace jreen
{

class DomCreater
{
	J_SINGLETON(DomCreater)
	static QDomDocument m_document;
public:
	inline QDomDocument *document() { return &m_document; }
	inline QDomElement createElement(const QString &name, const QString &value = QString());
};

inline QDomElement DomCreater::createElement(const QString &name, const QString &value)
{
	return jreen::createElement(&m_document, name, value);
}

}

#endif // DOMCREATER_P_H
