/*
 * %kadu copyright begin%
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2009 Juzef (juzefwt@tlen.pl)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2002, 2003 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2002, 2003 Dariusz Jagodzik (mast3r@kadu.net)
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

#ifndef BUDDY_DATA_WINDOW_H
#define BUDDY_DATA_WINDOW_H

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtGui/QWidget>

#include "buddies/buddy.h"

#include "exports.h"

class QCheckBox;
class QHostInfo;
class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QScrollArea;
class QTabWidget;
class QVBoxLayout;

class Buddy;
class BuddyGeneralConfigurationWidget;
class BuddyGroupsConfigurationWidget;
class BuddyPersonalInfoConfigurationWidget;
class BuddyOptionsConfigurationWidget;

class KADUAPI BuddyDataWindow : public QWidget
{
	Q_OBJECT

	Buddy MyBuddy;
	BuddyGeneralConfigurationWidget *ContactTab;
	BuddyGroupsConfigurationWidget *GroupsTab;
	BuddyPersonalInfoConfigurationWidget *PersonalInfoTab;
	BuddyOptionsConfigurationWidget *OptionsTab;

	QPushButton *OkButton;
	QPushButton *ApplyButton;

	void createGui();
	void createTabs(QLayout *layout);
	void createGeneralTab(QTabWidget *tabWidget);
	void createGroupsTab(QTabWidget *tabWidget);;
	void createPersonalInfoTab(QTabWidget *tabWidget);
	void createOptionsTab(QTabWidget *tabWidget);
	void createButtons(QLayout *layout);

	void keyPressEvent(QKeyEvent *);

	bool isValid();

private slots:
	void updateButtons();

	void updateBuddy();
	void updateBuddyAndClose();

public:
	explicit BuddyDataWindow(Buddy buddy, QWidget *parent = 0);
	virtual ~BuddyDataWindow();

};

#endif // BUDDY_DATA_WINDOW_H
