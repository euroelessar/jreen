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

#ifndef SASLFEATURE_H
#define SASLFEATURE_H

#include "streamfeature_p.h"
#include <QStringList>
#include <QtCrypto>

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
	public slots:
		void onClientStarted(bool init, const QByteArray &data);
		void onNextStep(const QByteArray &data);
		void onNeedParams(const QCA::SASL::Params &params);
		void onAuthCheck(const QString &user, const QString &authzid);
		void onError();
	private:
		void init();
		
		bool m_isSupported;
		int m_depth;
		State m_state;
		QStringList m_mechs;
		struct ScopedPointerEventDeleter
		{
			static inline void cleanup(QObject *pointer) { if (pointer) pointer->deleteLater(); }
		};
		QScopedPointer<QCA::SASL, ScopedPointerEventDeleter> m_sasl;
	};
}
#endif // SASLFEATURE_H
