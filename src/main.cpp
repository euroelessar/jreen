/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include <QtGui>
#include <QStringList>
#include "client.h"

using namespace jreen;

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	QStringList args = app.arguments();
	Client client(args.at(1), args.value(2), args.value(3, "-1").toInt());
	client.connectToServer();
	app.exec();
}
