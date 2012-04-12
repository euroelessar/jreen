/****************************************************************************
 *  xmlconsole.h
 *
 *  Copyright (c) 2009 by Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef XMLCONSOLE_H
#define XMLCONSOLE_H

#include <QtGui/QWidget>
#include "../jreen.h"

namespace Ui {
    class XmlConsole;
}

namespace Jreen
{

class XmlConsole : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY(XmlConsole)
public:
	explicit XmlConsole(QWidget *parent = 0);
    virtual ~XmlConsole();

protected:
    virtual void changeEvent(QEvent *e);

private:
    Ui::XmlConsole *m_ui;
};

}

#endif // XMLCONSOLE_H
