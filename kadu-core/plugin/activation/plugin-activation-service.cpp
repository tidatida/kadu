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

#include "plugin-activation-service.h"

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "misc/algorithm.h"
#include "misc/memory.h"
#include "misc/paths-provider.h"
#include "plugin/activation/active-plugin.h"
#include "plugin/activation/plugin-activation-error-exception.h"
#include "plugin/activation/plugin-activation-error-handler.h"
#include "plugin/plugin-dependency-handler.h"
#include "plugin/plugin-injector-provider.h"
#include "plugin/state/plugin-state-service.h"
#include "plugin/state/plugin-state.h"

PluginActivationService::PluginActivationService(QObject *parent) :
		QObject{parent}
{
}

PluginActivationService::~PluginActivationService()
{
}

void PluginActivationService::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void PluginActivationService::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void PluginActivationService::setPluginActivationErrorHandler(PluginActivationErrorHandler *pluginActivationErrorHandler)
{
	m_pluginActivationErrorHandler = pluginActivationErrorHandler;
}

void PluginActivationService::setPluginDependencyHandler(PluginDependencyHandler *pluginDependencyHandler)
{
	m_pluginDependencyHandler = pluginDependencyHandler;
}

void PluginActivationService::setPluginInjectorProvider(PluginInjectorProvider *pluginInjectorProvider)
{
	m_pluginInjectorProvider = pluginInjectorProvider;
}

void PluginActivationService::setPluginStateService(PluginStateService *pluginStateService)
{
	m_pluginStateService = pluginStateService;
}

QVector<QString> PluginActivationService::activatePluginWithDependencies(const QString &pluginName)
{
	if (isActive(pluginName) || !m_pluginDependencyHandler || !m_pluginStateService)
		return {};

	auto result = QVector<QString>{};
	try
	{
		auto withDependencies = m_pluginDependencyHandler->withDependencies(pluginName);
		if (withDependencies.isEmpty())
			throw PluginActivationErrorException(pluginName, tr("Plugin %1 not found").arg(pluginName));

		for (auto plugin : withDependencies)
		{
			auto conflict = findActiveProviding(m_pluginDependencyHandler->pluginMetadata(plugin).provides());
			if (!conflict.isEmpty() && conflict != plugin)
				throw PluginActivationErrorException(plugin, tr("Plugin %1 conflicts with: %2").arg(plugin, conflict));
		}

		for (auto plugin : withDependencies)
		{
			activatePlugin(plugin);
			result.append(plugin);
		}
	}
	catch (PluginActivationErrorException &e)
	{
		if (m_pluginActivationErrorHandler)
			m_pluginActivationErrorHandler->handleActivationError(e.pluginName(), e.errorMessage());
	}

	return result;
}

QString PluginActivationService::findActiveProviding(const QString &feature) const
{
	if (feature.isEmpty() || !m_pluginDependencyHandler)
		return {};

	for (auto const &activePlugin : m_activePlugins)
		if (m_pluginDependencyHandler->hasPluginMetadata(activePlugin.first))
			if (m_pluginDependencyHandler->pluginMetadata(activePlugin.first).provides() == feature)
				return activePlugin.first;

	return {};
}

QVector<QString> PluginActivationService::deactivatePluginWithDependents(const QString &pluginName)
{
	if (!isActive(pluginName) || !m_pluginDependencyHandler)
		return {};

	auto result = m_pluginDependencyHandler->withDependents(pluginName);
	for (auto const &plugin : result)
		deactivatePlugin(plugin);

	return result;
}

void PluginActivationService::activatePlugin(const QString &pluginName)
{
	if (!contains(m_activePlugins, pluginName))
	{
		m_activePlugins.insert(std::make_pair(pluginName, std::make_unique<ActivePlugin>(
			m_pathsProvider->pluginsLibPath(),
			m_pathsProvider->dataPath() + QStringLiteral("plugins/translations"),
			m_configuration->deprecatedApi()->readEntry("General", "Language"),
			pluginName,
			m_pluginInjectorProvider)));
		m_activePlugins[pluginName]->start();
	}
}

void PluginActivationService::deactivatePlugin(const QString &pluginName)
{
	m_activePlugins.erase(pluginName);
}

bool PluginActivationService::isActive(const QString &pluginName) const
{
	return contains(m_activePlugins, pluginName);
}

QSet<QString> PluginActivationService::activePlugins() const
{
	auto result = QSet<QString>{};
	for (auto const &activePlugin : m_activePlugins)
		result.insert(activePlugin.first);
	return result;
}

ActivePlugin * PluginActivationService::activePlugin(const QString& pluginName) const
{
	return isActive(pluginName)
			? m_activePlugins.at(pluginName).get()
			: nullptr;
}
