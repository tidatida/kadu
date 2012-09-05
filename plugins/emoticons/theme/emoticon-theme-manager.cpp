/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2004 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2002, 2003, 2004, 2005 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2005 Marcin Ślusarz (joi@kadu.net)
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

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>

#include "misc/kadu-paths.h"

#include "emoticon-theme-manager.h"

QString EmoticonThemeManager::defaultTheme()
{
#ifdef Q_WS_X11
	return QLatin1String("penguins");
#else
	return QLatin1String("tango");
#endif
}

bool EmoticonThemeManager::containsEmotsTxt(const QString &dir)
{
	QString kaduIconFileName = dir + "/emots.txt";
	QFileInfo kaduIconFile(kaduIconFileName);

	return kaduIconFile.exists();
}

EmoticonThemeManager::EmoticonThemeManager(QObject *parent) :
		ThemeManager(parent)
{
}

EmoticonThemeManager::~EmoticonThemeManager()
{
}

QString EmoticonThemeManager::defaultThemeName() const
{
	return defaultTheme();
}

QStringList EmoticonThemeManager::defaultThemePaths() const
{
	// Allow local themes to override global ones.
	QStringList result = getSubDirs(KaduPaths::instance()->profilePath() + QLatin1String("icons"));
	result += getSubDirs(KaduPaths::instance()->dataPath() + QLatin1String("themes/emoticons"));

	return result;
}

bool EmoticonThemeManager::isValidThemePath(const QString &themePath) const
{
	if (containsEmotsTxt(themePath))
		return true;

	QDir themeDir(themePath);
	QFileInfoList subDirs = themeDir.entryInfoList(QDir::Dirs);

	foreach (const QFileInfo &subDirInfo, subDirs)
	{
		if (!subDirInfo.fileName().startsWith('.'))
			if (containsEmotsTxt(subDirInfo.canonicalFilePath()))
				return true;
	}

	return false;
}
