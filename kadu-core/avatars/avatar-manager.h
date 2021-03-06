/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "avatars/avatar.h"
#include "storage/simple-manager.h"
#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class AccountManager;
class AvatarJobManager;
class AvatarService;
class AvatarStorage;
class Buddy;
class ContactManager;
class Contact;

class KADUAPI AvatarManager : public SimpleManager<Avatar>, AccountsAwareObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit AvatarManager(QObject *parent = nullptr);
	virtual ~AvatarManager();

	virtual QString storageNodeName() override { return QStringLiteral("Avatars"); }
	virtual QString storageNodeItemName() override { return QStringLiteral("Avatar"); }

	Avatar byBuddy(Buddy buddy, NotFoundAction action);
	Avatar byContact(Contact contact, NotFoundAction action);

	void updateAvatar(const Contact &contact, bool force = false);
	void removeAvatar(const Contact &contact);

signals:
	void avatarAboutToBeAdded(Avatar avatar);
	void avatarAdded(Avatar avatar);
	void avatarAboutToBeRemoved(Avatar avatar);
	void avatarRemoved(Avatar avatar);

	void avatarUpdated(Avatar avatar);

protected:
	virtual Avatar loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint) override;

	virtual void accountRegistered(Account account) override;
	virtual void accountUnregistered(Account account) override;

	virtual void itemAboutToBeAdded(Avatar item) override;
	virtual void itemAdded(Avatar item) override;
	virtual void itemAboutToBeRemoved(Avatar item) override;
	virtual void itemRemoved(Avatar item) override;

private:
	QPointer<AccountManager> m_accountManager;
	QPointer<AvatarJobManager> m_avatarJobManager;
	QPointer<AvatarStorage> m_avatarStorage;
	QPointer<ContactManager> m_contactManager;
	QTimer *UpdateTimer;

	bool needUpdate(const Contact &contact);

private slots:
	INJEQT_SET void setAccountManager(AccountManager *accountManager);
	INJEQT_SET void setAvatarJobManager(AvatarJobManager *avatarJobManager);
	INJEQT_SET void setAvatarStorage(AvatarStorage *avatarStorage);
	INJEQT_SET void setContactManager(ContactManager *contactManager);
	INJEQT_INIT void init();
	INJEQT_DONE void done();

	void avatarDataUpdated();
	void avatarPixmapUpdated();

	void updateAvatars();
	void updateAccountAvatars();
	void contactAdded(Contact contact);

};
