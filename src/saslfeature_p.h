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

#ifndef JREEN_SASLFEATURE_P_H
#define JREEN_SASLFEATURE_P_H

#include "streamfeature_p.h"
#include <QStringList>

#define USE_GSASL

#ifdef USE_GSASL
#  include <gsasl.h>
#else
extern "C"
{
#  include <sasl/sasl.h>
}
#endif

namespace Jreen
{
	class SASLFeature : public QObject, public StreamFeature
	{
		Q_OBJECT
	public:
		enum State {
			AtStart,
			AtMechanisms,
			AtMechanism,
			AtChallenge,
			AtSuccess
		};

		SASLFeature();
		int priority() { return 10; }
//		void setStreamInfo(StreamInfo *info);
		void reset();
		bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
		void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
		void handleEndElement(const QStringRef &name, const QStringRef &uri);
		void handleCharacterData(const QStringRef &text);
		bool isActivatable();
		bool activate();
	private:
#ifndef USE_GSASL
		void getParameters();
		void interactWithInfo(sasl_interact_t *interacts);
		void setInteractionResult(sasl_interact_t *interact, const QByteArray &value);
#endif

		int m_depth;
		State m_state;
		QStringList m_mechs;
#ifdef USE_GSASL
		struct SessionCleanup
		{
			static inline void cleanup(Gsasl_session *pointer) { if (pointer) gsasl_finish(pointer); }
		};
		QScopedPointer<Gsasl_session, SessionCleanup> m_session;
#else
		sasl_conn_t *m_session;
		QList<QByteArray> m_interact;
#endif
	};
}
#endif // JREEN_SASLFEATURE_P_H
