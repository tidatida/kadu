/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qcombobox.h>
#include <qmessagebox.h>
//Added by qt3to4:
#include <QPixmap>
#include <Q3ValueList>

#include "action.h"
#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"
#include "userbox.h"

IconsManager::IconsManager(const QString& name, const QString& configname) :
	Themes(name, configname, "icons_manager"), pixmaps(), icons()/*, animatedIcons()*///, menus()
{
	kdebugf();
	kdebugf2();
}

QString IconsManager::iconPath(const QString &name) const
{
	QString fname;
	if(name.contains('/'))
		fname = name;
	else
		fname = themePath() + getThemeEntry(name);
	return fname;
}

const QPixmap &IconsManager::loadPixmap(const QString &name)
{
	QMap<QString, QPixmap>::const_iterator i = pixmaps.find(name);
	if (i != pixmaps.end())
		return *i;

	QPixmap pix(iconPath(name));

	pixmaps.insert(name, pix);
	return pixmaps[name];
}

const QIcon &IconsManager::loadIcon(const QString &name)
{
	QMap<QString, QIcon>::const_iterator i = icons.find(name);
	if (i != icons.end())
		return *i;

	QIcon icon(iconPath(name));

	icons.insert(name, icon);
	return icons[name];

}

/*
const QMovie &IconsManager::loadAnimatedIcon(const QString &name)
{
	QMap<QString, QMovie>::const_iterator i = animatedIcons.find(name);
	if (i != animatedIcons.end())
		return *i;

	animatedIcons.insert(name, QMovie(iconPath(name)));
	return animatedIcons[name];
}*/

void IconsManager::clear()
{
	kdebugf();

	pixmaps.clear();
	icons.clear();

	kdebugf2();
}

// void IconsManager::registerMenu(QMenuData *menu)
// {
// 	kdebugf();
	//menus.push_front(qMakePair(menu, Q3ValueList<QPair<QString, QString> >()));
// 	kdebugf2();
// }

// void IconsManager::unregisterMenu(QMenuData *menu)
// {
// 	kdebugf();
// 	FOREACH(it, menus)
// 		if ((*it).first == menu)
// 		{
// 			menus.remove(it);
// 			break;
// 		}
// 	kdebugf2();
// }

// void IconsManager::registerMenuItem(QMenuData *menu, const QString &caption, const QString &iconName)
// {
// 	kdebugf();
// 	FOREACH(it, menus)
// 		if ((*it).first == menu)
// 		{
// 			(*it).second.push_front(qMakePair(caption, iconName));
// 			break;
// 		}
// 	kdebugf2();
// }

// void IconsManager::unregisterMenuItem(QMenuData *menu, const QString &caption)
// {
// 	kdebugf();
// 	FOREACH(it, menus)
// 		if ((*it).first == menu)
// 		{
// 			CONST_FOREACH(it2, (*it).second)
// 				if ((*it2).first == caption)
// 				{
// 					(*it).second.remove(*it2);
// 					break;
// 				}
// 			break;
// 		}
// 	kdebugf2();
// }

void IconsManager::refreshMenus()
{
	kdebugf();
// 	CONST_FOREACH(it, menus)
// 	{
// 		QMenuData *menu = (*it).first;
// 		for (unsigned int i = 0, count = menu->count(); i < count; ++i)
// 		{
// 			int id = menu->idAt(i);
// 			QString t = menu->text(id);
// 
// 			CONST_FOREACH(it2, (*it).second)
				//startsWith jest potrzebne, bo je�eli opcja w menu ma skr�t klawiszowy,
				//to menu->text(id) zwraca napis "Nazwa opcji\tskr�t klawiszowy"
// 				if (t == (*it2).first || t.startsWith((*it2).first + "\t"))
// 				{
// 					bool enabled = menu->isItemEnabled(id);
// 					bool visible = menu->isItemVisible(id);
// 					bool checked = menu->isItemChecked(id);
// 					menu->changeItem(id, loadIcon((*it2).second), t);
// 					menu->setItemEnabled(id, enabled);
// 					menu->setItemVisible(id, visible);
// 					menu->setItemChecked(id, checked);
// 				}
// 		}
// 	}
	kdebugf2();
}

// TODO: clear it!
void IconsManager::configurationUpdated()
{
	kdebugf();

	bool themeWasChanged = config_file.readEntry("Look", "IconTheme") != icons_manager->theme();

	icons_manager->clear();
	icons_manager->setTheme(config_file.readEntry("Look", "IconTheme"));
	KaduActions.refreshIcons();
	UserBox::userboxmenu->refreshIcons();
	UserBox::management->refreshIcons();
	icons_manager->refreshMenus();
	kadu->changeAppearance();

	// TODO: Make it standard
	if (themeWasChanged)
		emit themeChanged();

	kdebugf2();
}

void IconsManager::initModule()
{
	kdebugf();

	icons_manager = new IconsManager("icons", "icons.conf");

	icons_manager->setPaths(QStringList::split(QRegExp("(;|:)"), config_file.readEntry("Look", "IconsPaths")));

	QStringList themes = icons_manager->themes();
	QString theme = config_file.readEntry("Look", "IconTheme");
	if (!themes.isEmpty() && !themes.contains(theme))
	{
		theme = "default";
		config_file.writeEntry("Look", "IconTheme", "default");
	}

	icons_manager->setTheme(theme);

	kdebugf2();
}

void IconsManager::closeModule()
{
	kdebugf();

	delete icons_manager;
	icons_manager = 0;

	kdebugf2();
}

IconsManager *icons_manager;
