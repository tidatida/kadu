/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account-details.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/xml-configuration-file.h"
#include "buddies/buddy-manager.h"
#include "contacts/contact.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "protocols/protocols-manager.h"
#include "misc/misc.h"

#include "account.h"

Account Account::null(AccountShared::TypeNull);

Account Account::loadFromStorage(StoragePoint *accountStoragePoint)
{
	return Account(AccountShared::loadFromStorage(accountStoragePoint));
}

Account::Account(AccountShared::AccountType type) :
		Data(AccountShared::TypeNull != type ? new AccountShared(type) : 0)
{
	connectDataSignals();
}

Account::Account(AccountShared *data) :
		Data(data)
{
	connectDataSignals();
}

Account::Account(const Account &copy) :
		Data(copy.Data)
{
	connectDataSignals();
}

Account::~Account()
{
	disconnectDataSignals();
}

bool Account::isNull() const
{
	return !Data.data() || Data->isNull();
}

Account & Account::operator = (const Account &copy)
{
	disconnectDataSignals();
	Data = copy.Data;
	connectDataSignals();

	return *this;
}

bool Account::operator == (const Account &compare) const
{
	return Data == compare.Data;
}

bool Account::operator != (const Account &compare) const
{
	return Data != compare.Data;
}

int Account::operator < (const Account& compare) const
{
	return Data.data() - compare.Data.data();
}

void Account::connectDataSignals()
{
	if (isNull())
		return;

	connect(Data.data(), SIGNAL(buddyStatusChanged(Account, Buddy, Status)),
			this, SIGNAL(buddyStatusChanged(Account, Buddy, Status)));
}

void Account::disconnectDataSignals()
{
	if (isNull())
		return;

	disconnect(Data.data(), SIGNAL(buddyStatusChanged(Account, Buddy, Status)),
			this, SIGNAL(buddyStatusChanged(Account, Buddy, Status)));
}

QUuid Account::uuid() const
{
	return isNull() ? QUuid() : Data->uuid();
}

StoragePoint * Account::storage() const
{
	return isNull() ? 0 : Data->storage();
}

void Account::loadProtocol(ProtocolFactory *protocolFactory)
{
	if (isNull())
		return;

	Data->loadProtocol(protocolFactory);
}

void Account::unloadProtocol()
{
	if (Data)
		Data->unloadProtocol();
}

void Account::store()
{
	if (Data)
		Data->store();
}

void Account::removeFromStorage()
{
	if (Data)
		Data->removeFromStorage();
}

Buddy Account::getBuddyById(const QString& id)
{
	return BuddyManager::instance()->byId(*this, id);
}

Buddy Account::createAnonymous(const QString& id)
{
	if (!Data)
		return Buddy::null;

	Buddy result(BuddyShared::TypeAnonymous);
	ProtocolFactory *protocolFactory = Data->protocolHandler()->protocolFactory();
	ContactAccountData *contactAccountData = protocolFactory->newContactAccountData(*this, result, id);
	if (!contactAccountData->isValid())
	{
		delete contactAccountData;
		return Buddy::null;
	}

	result.addAccountData(contactAccountData);
	return result;
}

void Account::importProxySettings()
{
	if (!Data)
		return;

	Account defaultAccount = AccountManager::instance()->defaultAccount();
	if (!defaultAccount.isNull() && defaultAccount.proxyHost().toString() != "0.0.0.0")
	{
		Data->setUseProxy(defaultAccount.useProxy());
		Data->setProxyHost(defaultAccount.proxyHost());
		Data->setProxyPort(defaultAccount.proxyPort());
		Data->setProxyRequiresAuthentication(defaultAccount.proxyRequiresAuthentication());
		Data->setProxyUser(defaultAccount.proxyUser());
		Data->setProxyPassword(defaultAccount.proxyPassword());
	}
}

uint qHash(const Account &account)
{
	return qHash(account.uuid().toString());
}
