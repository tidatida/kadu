/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/windows/main-configuration-window.h"
#include "exports.h"

#include "configuration/gui/cenzor-configuration-ui-handler.h"
#include "notification/cenzor-notification.h"
#include "cenzor.h"

#include "cenzor-plugin.h"

CenzorPlugin::~CenzorPlugin()
{
}

bool CenzorPlugin::init(PluginRepository *pluginRepository, bool firstLoad)
{
	Q_UNUSED(pluginRepository)
	Q_UNUSED(firstLoad)

	Cenzor::createInstance();
	CenzorNotification::registerNotifications();
	CenzorConfigurationUiHandler::registerConfigurationUi();

	return true;
}

void CenzorPlugin::done()
{
	CenzorConfigurationUiHandler::unregisterConfigurationUi();
	CenzorNotification::unregisterNotifiactions();
	Cenzor::destroyInstance();
}

Q_EXPORT_PLUGIN2(cenzor, CenzorPlugin)

#include "moc_cenzor-plugin.cpp"
