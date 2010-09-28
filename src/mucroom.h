#ifndef MUCROOM_H
#define MUCROOM_H

#include <QObject>
#include <QDateTime>
#include "error.h"
#include "disco.h"

namespace jreen
{

class MucRoom : public QObject
{
	Q_OBJECT
public:
	enum Affiliation
	{
		AffiliationNone,                /**< No affiliation with the room. */
		AffiliationOutcast,             /**< The user has been banned from the room. */
		AffiliationMember,              /**< The user is a member of the room. */
		AffiliationOwner,               /**< The user is a room owner. */
		AffiliationAdmin,               /**< The user is a room admin. */
		AffiliationInvalid              /**< Invalid affiliation. */
	};
	enum Role
	{
		RoleNone,                       /**< Not present in room. */
		RoleVisitor,                    /**< The user visits a room. */
		RoleParticipant,                /**< The user has voice in a moderatd room. */
		RoleModerator,                  /**< The user is a room moderator. */
		RoleInvalid                     /**< Invalid role. */
	};
	enum HistoryRequest
	{
	};
	MucRoom( Client *client, const JID &jid );
//	inline void setPassword( const QString &password ) { m_password = password; }
//	inline const QString &name() const { return m_jid.node(); }
//	inline const QString &service() const { return m_jid.domain(); }
//	inline const QString &nick() const { return m_jid.resource(); }
	void join( Presence::Type type, const QString &message = QString(), int priority = 0 ) {}
//	void leave( const QString &message = QString() );
//	void send( const QString &message );
//	void setSubject( const QString &subject );
//	Affiliation affiliation() const { return m_affiliation; }
//	Role role() const { return m_role; }
//	void setNick( const QString &nick );
//	void setPresence( Presence::Type type, const QString &message = QString(), int priority = 0 );
//	void invite( const JID &jid, const QString &reason = QString(), const QString &thread = QString() );
//	void addHistory( const QString &message, const JID &from, const QDateTime &when );
//	void setRequestHistory( int value, HistoryRequest type );
//	void setRequestHistory( const QDateTime &since );
//	void requestVoice();
//	void kick( const QString &nick, const QString &reason = QString() ) { setRole( nick, RoleNone, reason ); }
//	void ban( const QString &nick, const QString &reason = QString() ) { setAffiliation( nick, AffiliationOutcast, reason ); }
//	void grantVoice( const QString &nick, const QString &reason = QString() ) { setRole( nick, RoleParticipant, reason ); }
//	void revokeVoice( const QString &nick, const QString &reason = QString() ) { setRole( nick, RoleVisitor, reason ); }
//	void setRole( const QString &nick, Role role, const QString &reason = QString() );
//	void setAffiliation( const QString &nick, Affiliation affiliation, const QString &reason = QString() );

public slots:
	inline void join() { join( Presence::Available ); }
//	void getRoomInfo();
//	void getRoomItems();
//	void requestRoomConfig();
signals:
	void newPresence( const Presence &presence );
	void newMessage( const Message &message, bool priv );
	void creation();
	void newSubject( const QString &nick, const QString &subject );
	void inviteDeclined( const JID &jid, const QString &reason );
	void newError( const Error &error );
	void newRoomInfo( QSet<QString> features, const QString &name, QSharedPointer<DataForm> form );
	void newRoomItems( const Disco::ItemList &items );
protected:
	JID m_jid;
	QString m_password;
	Affiliation m_affiliation;
	Role m_role;
};

}

#endif // MUCROOM_H
