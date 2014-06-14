/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration-storage.h"

#include "configuration/configuration.h"
#include "configuration/configuration-path-provider.h"
#include "file-system/atomic-file-write-exception.h"
#include "file-system/atomic-file-writer.h"
#include "debug.h"

#include <QtCore/QDir>

ConfigurationStorage::ConfigurationStorage(QObject *parent) :
		QObject{parent},
		m_configuration{nullptr},
		m_configurationPathProvider{nullptr}
{
}

ConfigurationStorage::~ConfigurationStorage()
{
}

void ConfigurationStorage::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void ConfigurationStorage::setConfigurationPathProvider(ConfigurationPathProvider *configurationPathProvider)
{
	m_configurationPathProvider = configurationPathProvider;
}

void ConfigurationStorage::write() const
{
	write(m_configurationPathProvider->configurationFilePath());
}

void ConfigurationStorage::backup() const
{
	write(m_configurationPathProvider->backupFilePath());
}

void ConfigurationStorage::write(const QString &fileName) const
{
	auto atomicFileWriter = AtomicFileWriter{};

	try
	{
		m_configuration->touch();
		atomicFileWriter.write(fileName, m_configuration->content());
	}
	catch (AtomicFileWriteException &)
	{
		kdebugm(KDEBUG_INFO, "error during saving of '%s'\n", qPrintable(fileName));
	}
}

#include "moc_configuration-storage.cpp"
