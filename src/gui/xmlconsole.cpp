/****************************************************************************
 *  xmlconsole.cpp
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

#include "xmlconsole.h"
#include "ui_xmlconsole.h"

namespace Jreen
{

XmlConsole::XmlConsole(QWidget *parent) :
	QWidget(parent),
	m_ui(new Ui::XmlConsole)
{
    m_ui->setupUi(this);
	setContentsMargins(0, 0, 0 ,0);
}

XmlConsole::~XmlConsole()
{
    delete m_ui;
}

void XmlConsole::changeEvent(QEvent *e)
{
	switch (e->type())
	{
    case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

}
