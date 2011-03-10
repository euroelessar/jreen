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

#ifndef SASLFEATURE_H
#define SASLFEATURE_H

#include "streamfeature.h"
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
