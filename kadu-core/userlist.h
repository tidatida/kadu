/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KADU_USERLIST_H
#define KADU_USERLIST_H

#include <QtCore/QMap>

#include "usergroup.h"
#include "exports.h"

/**
	\class UserList
	\brief Klasa reprezentuj�ca list� kontakt�w.
**/
class KADUAPI UserList : public UserGroup
{
	Q_OBJECT

	/**
		\fn UserList()
		Konstruktor standardowy.
	**/
	UserList();

	QMap<QString, QString> nonProtoKeys;
	QMap<QString, QMap<QString, QString> > protoKeys;

	void initKeys();

public:
	/**
		\fn static void initModule()
		Inicjuje modu� listy kontakt�w.
	**/
	static void initModule();

	/**
		\fn static void closeModule()
		Sprz�ta po module listy kontakt�w.
	**/
	static void closeModule();

	/**
		\fn ~UserList()
	**/
	virtual ~UserList();

public slots:
	/**
		\fn void merge(const QValueList<UserListElement> &list)
		Scala list� kontakt�w z podan� list� kontakt�w \a userlist.
		\param userlist lista kontakt�w z kt�r� nale�y scali�
	**/
	void merge(const QList<UserListElement> &list);

	/**
		\fn void setAllOffline(const QString &protocolName)
		\param protocolName identyfikator protoko�u
		Przestawia status wszystkich kontakt�w
		dla wskazanego protoko�u na "niedost�pny".
	**/
	void setAllOffline(const QString &protocolName);

	/**
		\fn void clear()
		Usuwa wszystkie kontakty.
	**/
	void clear();

	void addPerContactNonProtocolConfigEntry(const QString &attribute_name, const QString &internal_key);
	void removePerContactNonProtocolConfigEntry(const QString &attribute_name);
	void addPerContactProtocolConfigEntry(const QString &protocolName, const QString &attribute_name, const QString &internal_key);
	void removePerContactProtocolConfigEntry(const QString &protocolName, const QString &attribute_name);

};

extern KADUAPI UserList *userlist;

#endif
