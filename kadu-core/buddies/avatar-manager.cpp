/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QDir>
#include <QtCore/QFile>

#include "accounts/account.h"
#include "contacts/contact.h"
#include "misc/misc.h"
#include "protocols/protocol.h"
#include "protocols/services/avatar-service.h"

#include "avatar-manager.h"

AvatarManager * AvatarManager::Instance = 0;

AvatarManager * AvatarManager::instance()
{
	if (!Instance)
		Instance = new AvatarManager();

	return Instance;
}

AvatarManager::AvatarManager()
{
	triggerAllAccountsRegistered();
}

AvatarManager::~AvatarManager()
{
	triggerAllAccountsUnregistered();
}

AvatarService * AvatarManager::avatarService(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return 0;

	return protocol->avatarService();
}

AvatarService * AvatarManager::avatarService(Contact *contact)
{
	Account account = contact->account();
	if (account.isNull())
		return 0;

	return avatarService(account);
}

QString AvatarManager::avatarFileName(Avatar avatar)
{
	Contact *cad = avatar.contact();
	if (!cad)
		return QString::null;

	Account account = cad->account();
	if (account.isNull())
		return QString::null;

	return QString("%1-%2").arg(cad->buddy().uuid().toString(), account.uuid().toString());
}

void AvatarManager::accountRegistered(Account account)
{
	AvatarService *service = avatarService(account);
	if (!service)
		return;

	connect(service, SIGNAL(avatarFetched(Contact *, const QByteArray &)),
			this, SLOT(avatarFetched(Contact *, const QByteArray &)));
}

void AvatarManager::accountUnregistered(Account account)
{
	AvatarService *service = avatarService(account);
	if (!service)
		return;

	disconnect(service, SIGNAL(avatarFetched(Contact *, const QByteArray &)),
			   this, SLOT(avatarFetched(Contact *, const QByteArray &)));
}

void AvatarManager::updateAvatar(Contact *contact)
{
	QDateTime lastUpdated = contact->avatar().lastUpdated();
	QDateTime nextUpdate = contact->avatar().nextUpdate();
	if (lastUpdated.isValid() && lastUpdated.secsTo(QDateTime::currentDateTime()) < 60*60 || QFile::exists(contact->avatar().filePath()) && nextUpdate > QDateTime::currentDateTime())
		return;

	AvatarService *service = avatarService(contact);
	if (!service)
		return;

	service->fetchAvatar(contact);
}

void AvatarManager::avatarFetched(Contact *contact, const QByteArray &data)
{
	Avatar &avatar = contact->avatar();
	avatar.setLastUpdated(QDateTime::currentDateTime());

	QPixmap pixmap;
	pixmap.loadFromData(data);
	avatar.setPixmap(pixmap);

	QString avatarFile = avatarFileName(avatar);
	avatar.setFileName(avatarFile);

	QDir avatarsDir(ggPath("avatars"));
	if (!avatarsDir.exists())
		avatarsDir.mkpath(ggPath("avatars"));

	QFile file(avatarsDir.canonicalPath() + "/" + avatarFile);
	if (!file.open(QIODevice::WriteOnly))
		return;

	file.write(data);
	file.close();

	emit avatarUpdated(contact);
}
