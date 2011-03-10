/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef TUNE_H
#define TUNE_H

#include "stanzaextension.h"

namespace Jreen
{
	class TunePrivate;
	class JREEN_EXPORT Tune : public StanzaExtension
	{
		J_EXTENSION(Jreen::Tune, "")
		Q_DECLARE_PRIVATE(Tune)
	public:
		Tune();
		~Tune();
		
		void setArtist(const QString &);
		QString artist() const;
		void setLength(int);
		int length() const;
		void setRating(int );
		int rating() const;
		void setSource(const QString &);
		QString source() const;
		void setTitle(const QString &);
		QString title() const;
		void setTrack(const QString &);
		QString track() const;
		void setUri(const QUrl &);
		QUrl uri() const;
	private:
		QScopedPointer<TunePrivate> d_ptr;
	};
}

#endif // TUNE_H
