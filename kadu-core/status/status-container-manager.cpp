
/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/configuration-file.h"
#include "configuration/main-configuration.h"
#include "core/core.h"
#include "identities/identity-manager.h"
#include "status/status-container-aware-object.h"
#include "status/status-type-manager.h"
#include "icons-manager.h"

#include "status-container-manager.h"

StatusContainerManager * StatusContainerManager::Instance = 0;

KADUAPI StatusContainerManager * StatusContainerManager::instance()
{
	if (!Instance)
		Instance = new StatusContainerManager();
	return Instance;
}

StatusContainerManager::StatusContainerManager() :
		StatusContainer(0), SelfInList(false), DefaultStatusContainer(0)
{
	configurationUpdated();

	if (MainConfiguration::instance()->simpleMode())
		triggerAllIdentitiesAdded();
	else
		triggerAllAccountsRegistered();

	connect(MainConfiguration::instance(), SIGNAL(simpleModeChanged()), this, SLOT(simpleModeChanged()));
}

StatusContainerManager::~StatusContainerManager()
{
	disconnect(MainConfiguration::instance(), SIGNAL(simpleModeChanged()), this, SLOT(simpleModeChanged()));

	if (MainConfiguration::instance()->simpleMode())
		triggerAllIdentitiesRemoved();
	else
		triggerAllAccountsUnregistered();
}

void StatusContainerManager::accountRegistered(Account account)
{
	if (!MainConfiguration::instance()->simpleMode() && !StatusContainers.contains(account.statusContainer()))
		registerStatusContainer(account.statusContainer());
	
	if (MainConfiguration::instance()->simpleMode() && !StatusContainers.contains(account.accountIdentity()))
		registerStatusContainer(account.accountIdentity());
}

void StatusContainerManager::accountUnregistered(Account account)
{
	if (!MainConfiguration::instance()->simpleMode() && StatusContainers.contains(account.statusContainer()))
		unregisterStatusContainer(account.statusContainer());
}

void StatusContainerManager::identityAdded(Identity identity)
{
	if (MainConfiguration::instance()->simpleMode() && !StatusContainers.contains(identity) && identity.hasAnyAccount())
		registerStatusContainer(identity);
}

void StatusContainerManager::identityRemoved(Identity identity)
{
	if (MainConfiguration::instance()->simpleMode() && StatusContainers.contains(identity))
		unregisterStatusContainer(identity);
}

void StatusContainerManager::configurationUpdated()
{
	StartupStatus = config_file.readEntry("General", "StartupStatus");
	StartupLastDescription = config_file.readBoolEntry("General", "StartupLastDescription");
	StartupDescription = config_file.readEntry("General", "StartupDescription");
	OfflineToInvisible = config_file.readBoolEntry("General", "StartupStatusInvisibleWhenLastWasOffline");

	if (StartupStatus.isEmpty())
		StartupStatus = "LastStatus";
	else if (StartupStatus == "Busy")
		StartupStatus =  "Away";

	PrivateStatus = config_file.readBoolEntry("General", "PrivateStatus");

	DisconnectWithCurrentDescription = config_file.readBoolEntry("General", "DisconnectWithCurrentDescription");
	DisconnectDescription = config_file.readEntry("General", "DisconnectDescription");
}

void StatusContainerManager::cleanStatusContainers()
{
	while (!StatusContainers.isEmpty())
		unregisterStatusContainer(StatusContainers.first());
}

void StatusContainerManager::addAllAccounts()
{
	foreach (Account account, AccountManager::instance()->items())
		registerStatusContainer(account);
}

void StatusContainerManager::addAllIdentities()
{
	foreach (Identity identity, IdentityManager::instance()->items())
		if (MainConfiguration::instance()->simpleMode() && !StatusContainers.contains(identity) && identity.hasAnyAccount())
			registerStatusContainer(identity);
}

void StatusContainerManager::addSelfToList()
{
	if (SelfInList || !StatusContainers.isEmpty())
		return;

	registerStatusContainer(this);
	SelfInList = true;
}

void StatusContainerManager::removeSelfFromList()
{
	if (!SelfInList)
		return;

	unregisterStatusContainer(this);
	SelfInList = false;
}

void StatusContainerManager::setDefaultStatusContainer(StatusContainer *defaultStatusContainer)
{
	if (defaultStatusContainer == DefaultStatusContainer)
		return;

	if (DefaultStatusContainer)
		disconnect(DefaultStatusContainer, SIGNAL(statusChanged()), this, SIGNAL(statusChanged()));

	DefaultStatusContainer = defaultStatusContainer;

	if (DefaultStatusContainer && this != DefaultStatusContainer)
		connect(DefaultStatusContainer, SIGNAL(statusChanged()), this, SIGNAL(statusChanged()));
}

void StatusContainerManager::simpleModeChanged()
{
	cleanStatusContainers();
	if (MainConfiguration::instance()->simpleMode())
		addAllIdentities();
	else
		addAllAccounts();
}

void StatusContainerManager::registerStatusContainer(StatusContainer *statusContainer)
{
	removeSelfFromList();

	if (StatusContainers.isEmpty())
		setDefaultStatusContainer(statusContainer);

	emit statusContainerAboutToBeRegistered(statusContainer);
	StatusContainers.append(statusContainer);
	emit statusContainerRegistered(statusContainer);
	StatusContainerAwareObject::notifyStatusContainerRegistered(statusContainer);

	statusContainer->setDefaultStatus(StartupStatus, OfflineToInvisible, StartupDescription, StartupLastDescription);

	emitStatusContainerUpdated();
}

void StatusContainerManager::unregisterStatusContainer(StatusContainer *statusContainer)
{
	emit statusContainerAboutToBeUnregistered(statusContainer);
	StatusContainers.removeAll(statusContainer);
	emit statusContainerUnregistered(statusContainer);
	StatusContainerAwareObject::notifyStatusContainerUnregistered(statusContainer);

	addSelfToList();

	if (statusContainer == DefaultStatusContainer)
	{
		if (StatusContainers.isEmpty())
			DefaultStatusContainer = 0;
		else
			setDefaultStatusContainer(StatusContainers.first());
	}

	emitStatusContainerUpdated();
}

QString StatusContainerManager::statusContainerName()
{
	return tr("All");
}

void StatusContainerManager::setStatus(Status newStatus)
{
	foreach (StatusContainer *container, StatusContainers)
		container->setStatus(newStatus);
}

Status StatusContainerManager::status()
{
	return DefaultStatusContainer && this != DefaultStatusContainer
			? DefaultStatusContainer->status()
			: Status::null;
}

QString StatusContainerManager::statusName()
{
	return DefaultStatusContainer && this != DefaultStatusContainer
			? DefaultStatusContainer->statusName()
			: tr("Offline");
}

QIcon StatusContainerManager::statusIcon()
{
	return DefaultStatusContainer && this != DefaultStatusContainer
			? DefaultStatusContainer->statusIcon()
			: IconsManager::instance()->iconByPath("protocols/gadu-gadu/16x16/offline.png");
}

QIcon StatusContainerManager::statusIcon(Status status)
{
	return statusIcon(status.type());
}

QIcon StatusContainerManager::statusIcon(const QString &statusType)
{
	return DefaultStatusContainer && this != DefaultStatusContainer
			? DefaultStatusContainer->statusIcon(statusType)
			: QIcon();
// 			: IconsManager::instance()->loadPixmap(statusType);
}

QList<StatusType *> StatusContainerManager::supportedStatusTypes()
{
	return DefaultStatusContainer && this != DefaultStatusContainer
			? DefaultStatusContainer->supportedStatusTypes()
			: QList<StatusType *>();
// 			: StatusTypeManager::instance()->statusTypes();
}

int StatusContainerManager::maxDescriptionLength()
{
	return DefaultStatusContainer && this != DefaultStatusContainer
			? DefaultStatusContainer->maxDescriptionLength()
			: -1;
}

QString StatusContainerManager::statusNamePrefix()
{
	return QString(tr("All")) + " ";
}

void StatusContainerManager::storeStatus()
{
	foreach (StatusContainer *statusContainer, StatusContainers)
		statusContainer->storeStatus();
}

void StatusContainerManager::disconnectStatus(bool disconnectWithCurrentDescription, const QString& disconnectDescription)
{
	// TODO: 0.6.6
	Q_UNUSED(disconnectWithCurrentDescription)
	Q_UNUSED(disconnectDescription)

	foreach (StatusContainer *statusContainer, StatusContainers)
		statusContainer->disconnectStatus(DisconnectWithCurrentDescription, DisconnectDescription);
}

void StatusContainerManager::setPrivateStatus(bool isPrivate)
{
	foreach (StatusContainer *container, StatusContainers)
		container->setPrivateStatus(isPrivate);
}
