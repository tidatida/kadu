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

#include "emoticons-plugin-modules-factory.h"

#include "emoticons-module.h"

#include <injeqt/injector.h>

EmoticonsPluginModulesFactory::EmoticonsPluginModulesFactory(QObject *parent) :
		PluginModulesFactory{parent}
{
}

EmoticonsPluginModulesFactory::~EmoticonsPluginModulesFactory()
{
}

std::vector<std::unique_ptr<injeqt::module>> EmoticonsPluginModulesFactory::createPluginModules() const
{
	auto modules = std::vector<std::unique_ptr<injeqt::module>>{};
	modules.emplace_back(std::make_unique<EmoticonsModule>());

	return modules;
}

#include "moc_emoticons-plugin-modules-factory.cpp"
