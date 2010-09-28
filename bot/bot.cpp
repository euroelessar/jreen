#include "bot.h"
#include "disco.h"
#include <message.h>
#include <QDebug>
#include "gui/jidedit.h"
#include "util.h"
#include "nickname.h"

Bot::Bot() : m_client(new Client("jreen@qutim.org/JReen" ,"123coolpass"))
{
	m_client->setServer( "jabber.qutim.org" );
	m_client->setPresence( Presence::Online, "cool", 15 );
	m_client->disco()->setSoftwareVersion( "bot", "0.0.1", "Linux" );
	m_client->disco()->identities().append( Disco::Identity( "client", "type", "coolBot", "en" ) );
	m_client->disco()->identities().append( Disco::Identity( "client", "type", QString::fromUtf8("крутойБот"), "ru" ) );
	m_client->registerStanzaExtension( new Nickname );
	qDebug() << m_client->disco()->identities().first().category;
	qDebug() << m_client->disco()->identities().size();
	m_roster = new AbstractRoster( m_client );
	m_client->connectToServer();
	m_pinger.setInterval( 60000 );
	connect( &m_pinger, SIGNAL(timeout()), m_client, SLOT(whitespacePing()) );
	m_pinger.start();
	connect( m_client, SIGNAL(authorized()), m_client, SLOT(setPresence()) );
	connect( m_client, SIGNAL(authorized()), this, SLOT(sendMessage()) );
	connect( m_client, SIGNAL(authorized()), m_roster, SLOT(load()) );
//	QTimer::singleShot( 5000, m_client, SLOT(setPresence()) );
//	QTimer::singleShot( 1000, this, SLOT(sendMessage()) );
}

void Bot::sendMessage()
{
//	(new JidEdit())->show();
//	Message mes( Message::Chat, "torkvemada@jabber.ru", "Nyaaa!! ^_^" );
//	m_client->send( mes );
//	QTimer::singleShot( 60000, this, SLOT(sendMessage()) );
}
