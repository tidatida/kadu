/*
 * %kadu copyright begin%
 * Copyright 2009 Bartlomiej Zimon (uzi18@o2.pl)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "buddies/avatar-shared.h"
#include "contacts/contact.h"
#include "misc/path-conversion.h"

#include "avatar.h"

KaduSharedBaseClassImpl(Avatar)

Avatar Avatar::null;

Avatar Avatar::create()
{
	return new AvatarShared();
}

Avatar Avatar::loadFromStorage(StoragePoint *contactStoragePoint)
{
	return Avatar(AvatarShared::loadFromStorage(contactStoragePoint));
}

Avatar::Avatar()
{
}

Avatar::Avatar(AvatarShared *data) :
		SharedBase<AvatarShared>(data)
{
	data->ref.ref();
}

Avatar::Avatar(const Avatar &copy) :
		SharedBase<AvatarShared>(copy)
{
}

Avatar::~Avatar()
{
}

QString Avatar::filePath()
{
	return isNull() ? QString::null : data()->filePath();
}

KaduSharedBase_PropertyBoolReadDef(Avatar, Empty, true)
KaduSharedBase_PropertyDef(Avatar, Contact, avatarContact, AvatarContact, Contact::null)
KaduSharedBase_PropertyDef(Avatar, QDateTime, lastUpdated, LastUpdated, QDateTime())
KaduSharedBase_PropertyDef(Avatar, QDateTime, nextUpdate, NextUpdate, QDateTime())
KaduSharedBase_PropertyDef(Avatar, QPixmap, pixmap, Pixmap, QPixmap())
