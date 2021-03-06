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

#include "sound-actions.h"

#include "gui/sound-mute-action.h"
#include "sound-manager.h"

#include "gui/actions/actions.h"
#include "gui/menu/menu-inventory.h"
#include "plugin/plugin-injected-factory.h"

SoundActions::SoundActions(QObject *parent) :
		QObject{parent}
{
}

SoundActions::~SoundActions()
{
}

void SoundActions::setActions(Actions *actions)
{
	m_actions = actions;
}

void SoundActions::setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory)
{
	m_pluginInjectedFactory = pluginInjectedFactory;
}

void SoundActions::setMenuInventory(MenuInventory *menuInventory)
{
	m_menuInventory = menuInventory;
}

void SoundActions::setSoundManager(SoundManager *soundManager)
{
	m_soundManager = soundManager;
}

void SoundActions::init()
{
	m_soundMuteAction = m_pluginInjectedFactory->makeInjected<SoundMuteAction>(this);
	m_actions->insert(m_soundMuteAction);

	m_menuInventory
		->menu("main")
		->addAction(m_soundMuteAction, KaduMenu::SectionMiscTools, 7)
		->update();

	m_soundMuteAction->setSoundManager(m_soundManager);
	m_soundMuteAction->updateActionStates();
}

void SoundActions::done()
{
	m_menuInventory
		->menu("main")
		->removeAction(m_soundMuteAction)
		->update();
}

void SoundActions::configurationUpdated()
{
	m_soundMuteAction->updateActionStates();
}

#include "moc_sound-actions.cpp"
