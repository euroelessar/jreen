/****************************************************************************
**
** Jreen
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $JREEN_BEGIN_LICENSE$
** Jreen is free software: you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** Jreen is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with Jreen.  If not, see <http://www.gnu.org/licenses/>.
** $JREEN_END_LICENSE$
**
****************************************************************************/

#ifndef JREEN_ZLIBCOMPRESSIONFEATURE_P_H
#define JREEN_ZLIBCOMPRESSIONFEATURE_P_H

#include "streamfeature_p.h"
#include <QStringList>

namespace Jreen
{
	class ZLibCompressionFeature : public StreamFeature
	{
	public:
		ZLibCompressionFeature();
		int priority() { return 10; }
		void reset();
		bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
		void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
		void handleEndElement(const QStringRef &name, const QStringRef &uri);
		void handleCharacterData(const QStringRef &text);
		bool isActivatable();
		bool activate();
	private:
		enum State
		{
			AtMethods,
			AtMethod,
			AtStart
		};
		State m_state;
		int m_depth;
		QStringList m_methods;
	};
}

#endif // JREEN_ZLIBCOMPRESSIONFEATURE_P_H
