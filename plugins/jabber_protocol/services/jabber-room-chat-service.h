/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "protocols/services/account-service.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class JabberPresenceService;
class JabberRoomChat;

class BuddyManager;
class Chat;
class ChatDetailsRoom;
class ChatManager;
class ContactManager;
class MessageStorage;
class Message;

class QXmppClient;
class QXmppMessage;
class QXmppMucManager;

class JabberRoomChatService : public AccountService
{
	Q_OBJECT

public:
	explicit JabberRoomChatService(QXmppClient *client, QXmppMucManager *muc, Account account, QObject *parent = nullptr);
	virtual ~JabberRoomChatService();

	void setPresenceService(JabberPresenceService *presenceService);

	Message handleReceivedMessage(const QXmppMessage &xmppMessage) const;

	void joinOpenedRoomChats();
	bool isRoomChat(const Chat &chat) const;
	void leaveChat(const Chat &chat);

private:
	QPointer<QXmppClient> m_client;
	QPointer<QXmppMucManager> m_muc;
	QPointer<BuddyManager> m_buddyManager;
	QPointer<ChatManager> m_chatManager;
	QPointer<ContactManager> m_contactManager;
	QPointer<JabberPresenceService> m_presenceService;
	QPointer<MessageStorage> m_messageStorage;

	QMap<Chat, JabberRoomChat *> m_chats;

	JabberRoomChat * getRoomChat(const QString &id) const;
	JabberRoomChat * getRoomChat(const Chat &chat) const;
	JabberRoomChat * getOrCreateRoomChat(const Chat &chat);
	ChatDetailsRoom * myRoomChatDetails(const Chat &chat) const;

private slots:
	INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setContactManager(ContactManager *contactManager);
	INJEQT_SET void setMessageStorage(MessageStorage *messageStorage);
	INJEQT_INIT void init();

	void connected();

	void chatOpened(const Chat &chat);
	void chatClosed(const Chat &chat);

};
