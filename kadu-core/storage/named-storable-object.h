/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef NAMED_STORABLE_OBJECT_H
#define NAMED_STORABLE_OBJECT_H

#include "storable-object.h"

#include "exports.h"

/**
 * @addtogroup Storage
 * @{
 */

/**
 * @class NamedStorableObject
 * @author Rafal 'Vogel' Malinowski
 * @short Object that can load itself from XML file and store data there. Object is identified by string name.
 *
 * Many objects of this class can be stored under one master XML node in storage.
 * Each object is identified by unique name.
 */
class KADUAPI NamedStorableObject : public StorableObject
{

protected:
	virtual std::shared_ptr<StoragePoint> createStoragePoint();

public:
	NamedStorableObject();

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns name of this object.
	 * @return name of this object
	 *
	 * This method returns string that represents name of this object.
	 * Each derivered class should ovveride that method to return meaningfull name.
	 */
	virtual QString name() const = 0;

};

/**
 * @}
 */

#endif // NAMED_STORABLE_OBJECT_H
