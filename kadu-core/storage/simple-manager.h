/*
 * %kadu copyright begin%
 * Copyright 2011 Tomasz Rostanski (rozteck@interia.pl)
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SIMPLE_MANAGER_H
#define SIMPLE_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QObject>
#include <QtCore/QUuid>
#include <QtCore/QVector>

#include "storage/manager-common.h"
#include "storage/storable-object.h"

#include "exports.h"

/**
 * @addtogroup Storage
 * @{
 */

/**
 * @class SimpleManager
 * @author Rafal 'Vogel' Malinowski
 * @param Item class type of manager items, must be derivered from UuidStorableObject.
 * @short Object that manages instances of given Item type (including storing and loading from XML file).
 *
 * This object manages items of Item type - it allows adding, removing, searching by uuid,
 * index, loading and storing from XML configuration file.
 *
 * As a template, this class cannot be QObject and cannot emit signals. Each derivered class
 * that has to emits signals must also derive from QObject class and reimplement abstract
 * functions: @link itemAboutToBeAdded @endlink, @link itemAdded @endlink,
 * @link itemAboutToBeRemoved @endlink, @link itemRemoved @endlink to emit signals.
 *
 * Class Item must implement Item loadFromStorage(const std::shared_ptr\<StoragePoint\> &) static method.
 * Class Item must have static field Item null that represents unique NULL value.
 *
 * This class is thread-safe.
 */
template<class Item>
class SimpleManager : public StorableObject
{
	QMutex Mutex;

	QVector<Item> Items;

protected:
	SimpleManager(QObject *parent) :
			StorableObject{parent},
			Mutex{QMutex::Recursive}
	{
		setState(StateNotLoaded);
	}

	virtual ~SimpleManager()
	{
	}

	QMutex & mutex()
	{
		return Mutex;
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns storage parent of this object.
	 * @return storage parent of this object
	 *
	 * Returns NULL. This class stores configuration just under root node of
	 * XML configuration file.
	 */
	virtual StorableObject * storageParent()
	{
		return 0;
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns node name for items stored in this class.
	 * @return node name for items stored in this class
	 *
	 * Returns node name for items stored in this class.
	 */
	virtual QString storageNodeItemName() = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called just before item is added to manager.
	 *
	 * This method is called just before item is added to manager.
	 * Can be used to send a signal in derivered classes.
	 */
	virtual void itemAboutToBeAdded(Item item)
	{
		Q_UNUSED(item)
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called just after item is added to manager.
	 *
	 * This method is called just after item is added to manager.
	 * Can be used to send a signal in derivered classes.
	 */
	virtual void itemAdded(Item item)
	{
		Q_UNUSED(item)
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called just before item is removed from manager.
	 *
	 * This method is called just before item is removed from manager.
	 * Can be used to send a signal in derivered classes.
	 */
	virtual void itemAboutToBeRemoved(Item item)
	{
		Q_UNUSED(item)
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Method called just after item is removed from manager.
	 *
	 * This method is called just after item is removed from manager.
	 * Can be used to send a signal in derivered classes.
	 */
	virtual void itemRemoved(Item item)
	{
		Q_UNUSED(item)
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Loads all items from configuration file.
	 *
	 * Loads all items from configuration file. Uses @link storageNodeItemName @endlink
	 * to get name of subnodes. Items are loaded by Item::loadFromStorage static method.
	 * Every loads will cause @link itemAboutToBeAdded @endlink and @link itemAdded
	 * @endlink methods to be called.
	 */
	virtual void load()
	{
		QMutexLocker locker(&Mutex);

		if (!isValidStorage())
			return;

		StorableObject::load();

		QDomElement itemsNode = storage()->point();
		if (itemsNode.isNull())
			return;

		QVector<QDomElement> itemElements = storage()->storage()->getNodes(itemsNode, storageNodeItemName());
		Items.reserve(itemElements.count());

		foreach (const QDomElement &itemElement, itemElements)
		{
			auto storagePoint = std::make_shared<StoragePoint>(storage()->storage(), itemElement);
			QUuid uuid = storagePoint->point().attribute("uuid");
			if (!uuid.isNull())
			{
				Item item = loadStubFromStorage(storagePoint);
				Items.append(item);
			}
		}

		for (auto const &item : Items)
		{
			itemAboutToBeAdded(item);
			itemAdded(item);
		}

		loaded();
	}

	virtual Item loadStubFromStorage(const std::shared_ptr<StoragePoint> &storagePoint) = 0;

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Stores all items to configuration file.
	 *
	 * Stores all items to configuration file. Items are stored by their
	 * store method. If an item is new (was not loaded from storage) it creates its
	 * own node by its createStoragePoint method.
	 */
	virtual void store()
	{
		QMutexLocker locker(&Mutex);

		ensureLoaded();

		foreach (Item item, Items)
			item.ensureStored();
	}

public:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns item by index.
	 * @param index index of item to return
	 * @return item with given index
	 *
	 * Returns item with given index. Indexes are arbitrary and can
	 * change when items are added or removed. This method returns the
	 * same item as items().at(index) would return.
	 *
	 * When index is out of range Item::null value is returned.
	 */
	Item byIndex(int index)
	{
		QMutexLocker locker(&Mutex);

		ensureLoaded();

		if (index < 0 || index >= count())
			return Item::null;

		return Items.at(index);
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns item by uuid.
	 * @param uuid uuid of item to return
	 * @return item with given uuid
	 *
	 * Returns item with given uuid. Uuid are arbitrary but can not
	 * change (even between application restarts).
	 */
	Item byUuid(const QUuid &uuid)
	{
		QMutexLocker locker(&Mutex);

		ensureLoaded();

		if (uuid.isNull())
			return Item::null;

		foreach (const Item &item, Items)
			if (item.uuid() == uuid)
				return item;

		return Item::null;
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns index of given item.
	 * @param item item to find index
	 * @return index of given item
	 *
	 * Returns index of given item (or -1, if item is not found).
	 * Indexes are arbitrary and can change when items are added
	 * or removed. This method returns the same index as
	 * items().indexOf(item) would return.
	 */
	int indexOf(Item item)
	{
		QMutexLocker locker(&Mutex);

		ensureLoaded();
		return Items.indexOf(item);
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns number of loaded items.
	 * @return number of loaded items
	 *
	 * Return number of loaded items.
	 */
	int count()
	{
		QMutexLocker locker(&Mutex);

		ensureLoaded();
		return Items.count();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns list of items.
	 * @return list of items
	 *
	 * Return list of items.
	 */
	const QVector<Item> & items()
	{
		QMutexLocker locker(&Mutex);

		ensureLoaded();
		return Items;
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Adds new item to list.
	 * @param item new item to be added
	 *
	 * Adds new item to list. If item is already added this method does nothing.
	 * This method calls @link itemAboutToBeAdded @endlink and @link itemAdded
	 * @endlink virtual methods.
	 */
	void addItem(Item item)
	{
		QMutexLocker locker(&Mutex);

		ensureLoaded();

		if (Items.contains(item))
			return;

		itemAboutToBeAdded(item);
		Items.append(item);
		itemAdded(item);
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Removed item from list.
	 * @param item item to be removed
	 *
	 * Removes item from list. If item is not on list this method does nothing.
	 * This method calls @link itemAboutToBeRemoved @endlink and @link itemRemoved
	 * @endlink virtual methods.
	 *
	 * Item is removed from storage.
	 */
	void removeItem(Item item)
	{
		QMutexLocker locker(&Mutex);

		ensureLoaded();

		if (!Items.contains(item))
			return;

		itemAboutToBeRemoved(item);
		item.aboutToBeRemoved();

		Items.remove(Items.indexOf(item));

		item.remove();
		itemRemoved(item);
	}

};

/**
 * @}
 */

#endif // SIMPLE_MANAGER_H
