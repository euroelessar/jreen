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

#ifndef TLSFEATURE_H
#define TLSFEATURE_H

#include "streamfeature.h"
#include <QtCrypto>

namespace Jreen
{
class TLSFeature : public QObject, public StreamFeature
{
	Q_OBJECT
public:
	enum State {
		AtStart,
		AtFeature,
		AtChallenge,
		AtSuccess
	};
    TLSFeature();
	int priority() { return 10; }
//		void setStreamInfo(StreamInfo *info);
	void reset();
	bool canParse(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleStartElement(const QStringRef &name, const QStringRef &uri, const QXmlStreamAttributes &attributes);
	void handleEndElement(const QStringRef &name, const QStringRef &uri);
	void handleCharacterData(const QStringRef &text);
	virtual void setStreamInfo(StreamInfo *info);
	bool isActivatable();
	bool activate();
private slots:
	void onHandshaken();
	void onClosed();
	void onError();
	void onDisconnected();
private:
	void init();
	
	struct ScopedPointerEventDeleter
	{
		static inline void cleanup(QObject *pointer) { if (pointer) pointer->deleteLater(); }
	};
	QScopedPointer<QCA::TLS, ScopedPointerEventDeleter> m_tls;
	bool m_hasTls;
	bool m_required;
	bool m_available;
};
}

#endif // TLSFEATURE_H
