/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/accounts-aware-object.h"
#include "configuration/configuration-file.h"
#include "configuration/configuration-manager.h"
#include "configuration/xml-configuration-file.h"
#include "core/core.h"
#include "notify/notification-manager.h"
#include "protocols/connection-error-notification.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"
#include "debug.h"

#include "account-manager.h"

AccountManager * AccountManager::Instance = 0;

KADUAPI AccountManager * AccountManager::instance()
{
	if (0 == Instance)
	{
		Instance = new AccountManager();
		Instance->init();
	}
	return Instance;
}

AccountManager::AccountManager() :
		StorableObject(true)
{
	Core::instance()->configuration()->registerStorableObject(this);
}

AccountManager::~AccountManager()
{
	foreach (ProtocolFactory *factory, ProtocolsManager::instance()->protocolFactories())
		protocolFactoryUnregistered(factory);

	disconnect(ProtocolsManager::instance(), SIGNAL(protocolFactoryRegistered(ProtocolFactory *)),
			this, SLOT(protocolFactoryRegistered(ProtocolFactory *)));
	disconnect(ProtocolsManager::instance(), SIGNAL(protocolFactoryUnregistered(ProtocolFactory *)),
			this, SLOT(protocolFactoryUnregistered(ProtocolFactory *)));

	Core::instance()->configuration()->unregisterStorableObject(this);
}

void AccountManager::init()
{
	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryRegistered(ProtocolFactory *)),
			this, SLOT(protocolFactoryRegistered(ProtocolFactory *)));
	connect(ProtocolsManager::instance(), SIGNAL(protocolFactoryUnregistered(ProtocolFactory *)),
			this, SLOT(protocolFactoryUnregistered(ProtocolFactory *)));

	foreach (ProtocolFactory *factory, ProtocolsManager::instance()->protocolFactories())
		protocolFactoryRegistered(factory);
}

void AccountManager::loadAccount(Account account, ProtocolFactory *protocolFactory)
{
	account.loadProtocol(protocolFactory);
	registerAccount(account);
}

void AccountManager::unloadAccount(Account account)
{
	unregisterAccount(account);
	account.unloadProtocol();
}

void AccountManager::tryLoadAccount(Account account)
{
	ProtocolFactory *factory = ProtocolsManager::instance()->byName(account.protocolName());
	if (factory)
		loadAccount(account, factory);
}

StoragePoint * AccountManager::createStoragePoint()
{
	return new StoragePoint(xml_config_file, xml_config_file->getNode("Accounts"));
}

void AccountManager::load()
{
	if (!isValidStorage())
		return;

	QDomElement accountsNode = storage()->point();
	if (accountsNode.isNull())
		return;

	QList<QDomElement> accountElements = storage()->storage()->getNodes(accountsNode, "Account");
	foreach (QDomElement accountElement, accountElements)
	{
		StoragePoint *storagePoint = new StoragePoint(storage()->storage(), accountElement);
		Account account = Account::loadFromStorage(storagePoint);
		AllAccounts.append(account);

		tryLoadAccount(account);
	}
}

void AccountManager::store()
{
	if (!isValidStorage())
		return;
	
	foreach (Account account, AllAccounts)
		account.store();
}

Account AccountManager::defaultAccount() const
{
	return byIndex(0);
}

Account AccountManager::byIndex(unsigned int index) const
{
	if (index < 0 || index >= count())
		return Account::null;

	return RegisteredAccounts.at(index);
}

Account AccountManager::byUuid(const QUuid &uuid) const
{
	foreach (Account account, AllAccounts)
		if (uuid == account.uuid())
			return account;

	return Account::null;
}

const QList<Account> AccountManager::byProtocolName(const QString &name) const
{
	QList<Account> list;
	foreach (Account account, AllAccounts)
		if (account.protocolName() == name)
			list.append(account);

	return list;
}

void AccountManager::registerAccount(Account account)
{
	emit accountAboutToBeRegistered(account);
	RegisteredAccounts << account;
	emit accountRegistered(account);
	AccountsAwareObject::notifyAccountRegistered(account);

	connect(account.protocolHandler(), SIGNAL(connectionError(Account, const QString &, const QString &)),
			this, SLOT(connectionError(Account, const QString &, const QString &)));
}

void AccountManager::unregisterAccount(Account account)
{
	disconnect(account.protocolHandler(), SIGNAL(connectionError(Account, const QString &, const QString &)),
			this, SLOT(connectionError(Account, const QString &, const QString &)));

	AccountsAwareObject::notifyAccountUnregistered(account);
	emit accountAboutToBeUnregistered(account);
	RegisteredAccounts.removeAll(account);
	emit accountUnregistered(account);
}

void AccountManager::deleteAccount(Account account)
{
	emit accountAboutToBeRemoved(account);
	unregisterAccount(account);
	account.removeFromStorage();
	emit accountRemoved(account);
}

Status AccountManager::status() const
{
	Account account = defaultAccount();
	return !account.isNull()
			? account.statusContainer()->status()
			: Status();
}

void AccountManager::protocolFactoryRegistered(ProtocolFactory *factory)
{
	if (!isValidStorage())
		return;

	QString factoryProtocolName = factory->name();

	foreach (Account account, AllAccounts)
		if (account.protocolName() == factoryProtocolName)
			loadAccount(account, factory);
}

void AccountManager::protocolFactoryUnregistered(ProtocolFactory *factory)
{
	foreach (Account account, RegisteredAccounts)
		if (account.protocolHandler()->protocolFactory() == factory)
			unloadAccount(account);
}

void AccountManager::connectionError(Account account, const QString &server, const QString &message)
{
	kdebugf();

	if (!ConnectionErrorNotification::activeError(account, message))
	{
		ConnectionErrorNotification *connectionErrorNotification = new ConnectionErrorNotification(account,
				server, message);
		NotificationManager::instance()->notify(connectionErrorNotification);
	}

	kdebugf2();
}
