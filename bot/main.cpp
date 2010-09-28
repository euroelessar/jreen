#include <QtCore>
#include <QtGui>
#include <QDebug>
#include "bot.h"

#define DO_COPYRIGHT

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	Bot b;
#ifdef DO_COPYRIGHT
	QStringList sources = QStringList() << "/../src" << "/../src/gui" ;
	QFileInfoList source_files;
	foreach( const QString &path, sources )
		source_files << QDir( a.applicationDirPath() + path ).entryInfoList( QStringList() << "*.cpp" << "*.h" );
	QRegExp rx( "\\nclass.*\\n" );
	rx.setMinimal( true );
	foreach( QFileInfo source_file, source_files )
	{
		QFile file( source_file.absoluteFilePath() );
		file.open( QIODevice::ReadOnly );
		QString text = QString::fromUtf8( file.readAll() );
		file.close();
		if( !text.startsWith( '/' ) )
		{
			QString copyright( "/****************************************************************************\n"
			" *  %1\n"
			" *\n"
			" *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>\n"
			" *\n"
			" ***************************************************************************\n"
			" *                                                                         *\n"
			" *   This library is free software; you can redistribute it and/or modify  *\n"
			" *   it under the terms of the GNU General Public License as published by  *\n"
			" *   the Free Software Foundation; either version 2 of the License, or     *\n"
			" *   (at your option) any later version.                                   *\n"
			" *                                                                         *\n"
			" ***************************************************************************\n"
			"*****************************************************************************/\n\n" );
			text = text.prepend( copyright.arg( source_file.fileName() ) );
			file.open( QIODevice::WriteOnly );
			file.write( text.toUtf8() );
			file.close();
		}
		if( source_file.fileName().contains( "_p.h" ) || source_file.fileName().contains( ".cpp" ) )
			continue;
		int pos = 0;
		while ((pos = rx.indexIn(text, pos + 1)) != -1)
		{
			QString line = rx.cap( 0 );
			if( line.indexOf( ';' ) > -1 )
				continue;
			if( line.indexOf( ':' ) > -1 )
				line.truncate( line.indexOf( ':' ) );
			line = line.trimmed();
			QFile file( a.applicationDirPath() + "/../include/jreen/" + line.section( ' ', -1, -1 ) );
			file.open( QIODevice::WriteOnly );
			file.write( QString("#include \"%1\"\n").arg( source_file.fileName() ).toUtf8() );
			file.close();
		}
	}
#endif
    return a.exec();
}
