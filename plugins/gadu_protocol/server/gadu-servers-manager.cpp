/*
 * %kadu copyright begin%
 * Copyright 2011, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include <QtCore/QFile>
#include <QtCore/QRegExp>

#include <libgadu.h>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "misc/paths-provider.h"

#include "gadu-servers-manager.h"

GaduServersManager::GaduServersManager(QObject *parent) :
		QObject{parent}
{
}

GaduServersManager::~GaduServersManager()
{
}

void GaduServersManager::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void GaduServersManager::setPathsProvider(PathsProvider *pathsProvider)
{
	m_pathsProvider = pathsProvider;
}

void GaduServersManager::init()
{
	buildServerList();
}

QList<GaduServersManager::GaduServer> GaduServersManager::gaduServersFromString(const QString &serverAddress)
{
	QList<GaduServer> result;

	if (serverAddress.isEmpty() || serverAddress.startsWith(QStringLiteral("0.0.0.0")))
		return result;

	QString address;
	QList<int> ports;
	QRegExp addressPortRegexp( "^(.+):(\\d+)$" ); // X:Y
	if (serverAddress.contains(addressPortRegexp))
	{
		address = addressPortRegexp.cap(1);
		int port = addressPortRegexp.cap(2).toInt();
		ports << port;
	}
	else
	{
		address = serverAddress;
		ports = AllPorts;
	}

	QList<QString> servers;
	QRegExp ipRangeRegexp("^(\\d+)\\.(\\d+)\\.(\\d+)\\.(\\d+)-(\\d+)$"); // X.X.X.X-X
	if (address.contains(ipRangeRegexp))
	{
		int a = ipRangeRegexp.cap(1).toInt();
		int b = ipRangeRegexp.cap(2).toInt();
		int c = ipRangeRegexp.cap(3).toInt();
		int d1 = ipRangeRegexp.cap(4).toInt();
		int d2 = ipRangeRegexp.cap(5).toInt();
		for (int d = d1; d <= d2; ++d)
			servers << QString("%1.%2.%3.%4").arg(a).arg(b).arg(c).arg(d);
	}
	else
		servers << address;

	QHostAddress ip;
	foreach (const QString &server, servers)
		if (ip.setAddress(server))
			foreach (int port, ports)
				result.append(GaduServer(ip, port));

	return result;
}

void GaduServersManager::loadServerListFromFile(const QString &fileName)
{
	GoodServers << GaduServer(QHostAddress((quint32)0), 0); // for GG hub
	GoodServers << gaduServersFromString(m_configuration->deprecatedApi()->readEntry("Network", "LastServerIP"));

	QFile file(fileName);

	if (!file.open(QFile::ReadOnly))
		return;

	QTextStream serversStream(&file);

	while (!serversStream.atEnd())
	{
		QString server = serversStream.readLine();
		GoodServers << gaduServersFromString(server);
	}

	file.close();
}

void GaduServersManager::loadServerListFromString(const QString& data)
{
	QStringList servers = data.split(';', QString::SkipEmptyParts);

	foreach (const QString &server, servers)
		GoodServers << gaduServersFromString(server.trimmed());
	GoodServers << GaduServer(QHostAddress((quint32)0), 0); // for GG hub
	GoodServers << gaduServersFromString(m_configuration->deprecatedApi()->readEntry("Network", "LastServerIP"));
}

void GaduServersManager::buildServerList()
{
	GoodServers.clear();
	BadServers.clear();
	AllServers.clear();
	AllPorts.clear();

	int LastGoodPort = m_configuration->deprecatedApi()->readNumEntry("Network", "LastServerPort",
			m_configuration->deprecatedApi()->readNumEntry("Network", "DefaultPort", 443));

	if (8074 == LastGoodPort || 443 == LastGoodPort)
		AllPorts << LastGoodPort;
	if (8074 != LastGoodPort)
		AllPorts << 8074;
	if (443 != LastGoodPort)
		AllPorts << 443;

	if (m_configuration->deprecatedApi()->readBoolEntry("Network", "isDefServers", true))
		loadServerListFromFile(m_pathsProvider->dataPath() + QStringLiteral("plugins/data/gadu_protocol/servers.txt"));
	else
		loadServerListFromString(m_configuration->deprecatedApi()->readEntry("Network", "Server"));

	AllServers = GoodServers;
}

void GaduServersManager::configurationUpdated()
{
	buildServerList();
}

GaduServersManager::GaduServer GaduServersManager::getServer(bool onlyTls)
{
	if (GoodServers.isEmpty())
	{
		GoodServers = BadServers;
		BadServers.clear();
		return GaduServer(QHostAddress(), 0);
	}

	if (onlyTls && GoodServers[0].second != 443 && GoodServers[0].second != 0)
	{
		markServerAsBad(GoodServers[0]);
		return getServer(true);
	}

	return GoodServers[0];
}

const QList<GaduServersManager::GaduServer> & GaduServersManager::getServersList()
{
	return AllServers;
}

void GaduServersManager::markServerAsGood(GaduServersManager::GaduServer server)
{
	m_configuration->deprecatedApi()->writeEntry("Network", "LastServerIP", server.first.toString());
	m_configuration->deprecatedApi()->writeEntry("Network", "LastServerPort", server.second);
}

void GaduServersManager::markServerAsBad(GaduServersManager::GaduServer server)
{
	GoodServers.removeAll(server);
	BadServers.append(server);
}
