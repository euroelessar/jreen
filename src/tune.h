/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef TUNE_H
#define TUNE_H

#include "stanzaextension.h"

namespace Jreen
{
	class TunePrivate;
	class JREEN_EXPORT Tune : public Payload
	{
		J_PAYLOAD(Jreen::Tune)
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
