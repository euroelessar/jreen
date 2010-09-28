#ifndef BOT_H
#define BOT_H

#include <QtCore>
#include <client.h>
#include <jid.h>
#include <abstractroster.h>

J_USE_NAMESPACE

class Bot : public QObject
{
	Q_OBJECT
	Client *m_client;
	QTimer m_pinger;
	AbstractRoster *m_roster;
public:
	Bot();
public slots:
	void sendMessage();
};

#endif // BOT_H
