/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/accounts-aware-object.h"
#include "contacts/contact.h"
#include "injeqt-type-roles.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class AccountManager;
class ChatManager;
class ChatStorage;
class InjectedFactory;
class Message;
class MultilogonSession;
class NotificationService;
class StatusTypeManager;
class Status;

class AccountEventListener : public QObject, AccountsAwareObject
{
	Q_OBJECT
	INJEQT_TYPE_ROLE(LISTENER)

public:
	Q_INVOKABLE explicit AccountEventListener(QObject *parent = nullptr);
	virtual ~AccountEventListener();

protected:
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<ChatManager> m_chatManager;
	QPointer<ChatStorage> m_chatStorage;
	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<NotificationService> m_notificationService;
	QPointer<StatusTypeManager> m_statusTypeManager;

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setChatManager(ChatManager *chatManager);
	INJEQT_SET void setChatStorage(ChatStorage *chatStorage);
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setNotificationService(NotificationService *notificationService);
	INJEQT_SET void setStatusTypeManager(StatusTypeManager *statusTypeManager);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	void multilogonSessionConnected(MultilogonSession *session);
	void multilogonSessionDisconnected(MultilogonSession *session);
	void contactStatusChanged(Contact contact, Status oldStatus);
	void accountConnected();

};
