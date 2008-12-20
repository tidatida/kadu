/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTACT_LIST_H
#define CONTACT_LIST_H

#include <QtCore/QList>

#include "contact.h"

class ContactList : public QList<Contact>
{

public:
	bool operator == (ContactList &compare);

};

#endif // CONTACT_LIST_H
