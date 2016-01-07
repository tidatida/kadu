/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "cenzor-plugin-object.h"

#include "configuration/gui/cenzor-configuration-ui-handler.h"
#include "cenzor-message-filter.h"

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "gui/windows/main-configuration-window.h"
#include "message/message-filter-service.h"
#include "misc/paths-provider.h"
#include "notification/notification-event-repository.h"
#include "notification/notification-event.h"
#include "status/status-changer-manager.h"

CenzorPluginObject::CenzorPluginObject(QObject *parent) :
		QObject{parent}
{
}

CenzorPluginObject::~CenzorPluginObject()
{
}

void CenzorPluginObject::setCenzorConfigurationUiHandler(CenzorConfigurationUiHandler *cenzorConfigurationUiHandler)
{
	m_cenzorConfigurationUiHandler = cenzorConfigurationUiHandler;
}

void CenzorPluginObject::setCenzorMessageFilter(CenzorMessageFilter *cenzorMessageFilter)
{
	m_cenzorMessageFilter = cenzorMessageFilter;
}

void CenzorPluginObject::setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository)
{
	m_configurationUiHandlerRepository = configurationUiHandlerRepository;
}

void CenzorPluginObject::setMessageFilterService(MessageFilterService *messageFilterService)
{
	m_messageFilterService = messageFilterService;
}

void CenzorPluginObject::setNotificationEventRepository(NotificationEventRepository *notificationEventRepository)
{
	m_notificationEventRepository = notificationEventRepository;
}

void CenzorPluginObject::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void CenzorPluginObject::init()
{
	m_configurationUiHandlerRepository->addConfigurationUiHandler(m_cenzorConfigurationUiHandler);
	m_messageFilterService->registerMessageFilter(m_cenzorMessageFilter);
	m_notificationEventRepository->addNotificationEvent(NotificationEvent{"cenzorNotification", QT_TRANSLATE_NOOP("@default", "Message was cenzored")});
	MainConfigurationWindow::registerUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/cenzor.ui"));
}

void CenzorPluginObject::done()
{
	MainConfigurationWindow::unregisterUiFile(m_pathsProvider->dataPath() + QLatin1String("plugins/configuration/cenzor.ui"));
	m_notificationEventRepository->removeNotificationEvent(NotificationEvent{"cenzorNotification", QT_TRANSLATE_NOOP("@default", "Message was cenzored")});
	m_messageFilterService->unregisterMessageFilter(m_cenzorMessageFilter);
	m_configurationUiHandlerRepository->removeConfigurationUiHandler(m_cenzorConfigurationUiHandler);
}

#include "moc_cenzor-plugin-object.cpp"