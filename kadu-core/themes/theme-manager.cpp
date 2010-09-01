/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include <QtCore/QStringList>

#include "theme-manager.h"

ThemeManager::ThemeManager(QObject *parent) :
		QObject(parent), CurrentThemeIndex(-1)
{
}

ThemeManager::~ThemeManager()
{
}

int ThemeManager::getDefaultThemeIndex()
{
	for (int i = 0; i < Themes.size(); i++)
		if (Themes.at(i).name() == "default")
			return i;

	return -1;
}

QStringList ThemeManager::getSubDirs(const QString &dirPath)
{
	QDir dir(dirPath);
	QStringList subDirs = dir.entryList(QDir::Dirs);

	QStringList result;
	foreach (const QString &subDir, subDirs)
		if (!subDir.startsWith('.')) // ignore hidden, this, and parent
			result.append(dirPath + '/' + subDir);

	return result;
}

void ThemeManager::loadThemes(QStringList pathList)
{
	pathList = pathList + defaultThemePathes();

	QString currentThemeName = currentTheme().name();
	CurrentThemeIndex = -1;

	Themes.clear();

	foreach (const QString &path, pathList)
	{
		if (!isValidThemePath(path))
			continue;

		QString newThemeName = getThemeName(path);
		Theme theme(path + "/", newThemeName);
		Themes.append(theme);

		if (newThemeName == currentThemeName)
			CurrentThemeIndex = Themes.size() - 1;
	}

	if (-1 == CurrentThemeIndex)
		CurrentThemeIndex = getDefaultThemeIndex();

	emit themeListUpdated();
}

void ThemeManager::setCurrentTheme(const QString &themePath)
{
	for (int i = 0; i < Themes.size(); i++)
	{
		const Theme &theme = Themes.at(i);

		if (themePath == theme.name() || themePath == theme.path())
		{
			CurrentThemeIndex = i;
			printf("cti set to: %d\n", i);
			return;
		}
	}

	CurrentThemeIndex = getDefaultThemeIndex();
	printf("cti set to: %d\n", CurrentThemeIndex);
}

int ThemeManager::currentThemeIndex() const
{
	return CurrentThemeIndex;
}

const Theme & ThemeManager::currentTheme() const
{
	if (CurrentThemeIndex < 0 || CurrentThemeIndex >= Themes.size())
		return Theme::null;

	return Themes.at(CurrentThemeIndex);
}
