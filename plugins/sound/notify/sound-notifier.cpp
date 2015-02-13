/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "notify/notification/notification.h"

#include "configuration/gui/sound-configuration-ui-handler.h"
#include "sound-manager.h"

#include "sound-notifier.h"

SoundNotifier * SoundNotifier::Instance = 0;

void SoundNotifier::createInstance()
{
	if (!Instance)
		Instance = new SoundNotifier();
}

void SoundNotifier::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

SoundNotifier * SoundNotifier::instance()
{
	return Instance;
}

SoundNotifier::SoundNotifier() :
		Notifier("Sound", QT_TRANSLATE_NOOP("@default", "Play a sound"), KaduIcon("audio-volume-high"))
{
}

SoundNotifier::~SoundNotifier()
{
}

void SoundNotifier::setManager(SoundManager *manager)
{
	m_manager = manager;
}

void SoundNotifier::notify(Notification *notification)
{
	m_manager->playSoundByName(notification->key());
}

NotifierConfigurationWidget * SoundNotifier::createConfigurationWidget(QWidget* parent)
{
	return SoundConfigurationUiHandler::instance()->createConfigurationWidget(parent);
}

#include "moc_sound-notifier.cpp"
