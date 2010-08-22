/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDIES_LIST_VIEW_DELEGATE_CONFIGURATION_H
#define BUDDIES_LIST_VIEW_DELEGATE_CONFIGURATION_H

#include <QtGui/QColor>
#include <QtGui/QFont>
#include <QtGui/QPixmap>

#include "configuration/configuration-aware-object.h"

class QListView;

class BuddiesListViewDelegateConfiguration : private ConfigurationAwareObject
{
	Q_DISABLE_COPY(BuddiesListViewDelegateConfiguration)

	QListView *ListView;

	QFont Font;
	QFont BoldFont;
	QFont DescriptionFont;

	bool AlignTop;
	bool ShowAccountName;
	bool ShowBold;
	bool ShowDescription;
	bool ShowMultiLineDescription;
	bool ShowAvatars;
	bool AvatarBorder;
	bool AvatarGreyOut;
	QColor DescriptionColor;

	QSize DefaultAvatarSize;
	QPixmap MessagePixmap;

protected:
	virtual void configurationUpdated();

public:
	BuddiesListViewDelegateConfiguration(QObject *listView);

	void setShowAccountName(bool showAccountName) { ShowAccountName = showAccountName; }

	QFont font() const { return Font; }
	QFont boldFont() const { return BoldFont; }
	QFont descriptionFont() const { return DescriptionFont; }

	bool alignTop() const { return AlignTop; }
	bool showAccountName() const { return ShowAccountName; }
	bool showBold() const { return ShowBold; }
	bool showDescription() const { return ShowDescription; }
	bool showMultiLineDescription() const { return ShowMultiLineDescription; }
	bool showAvatars() const { return ShowAvatars; }
	bool avatarBorder() const { return AvatarBorder; }
	bool avatarGreyOut() const { return AvatarGreyOut; }
	QColor descriptionColor() const { return DescriptionColor; }

	QSize defaultAvatarSize() const { return DefaultAvatarSize; }
	QPixmap messagePixmap() const { return MessagePixmap; }

};

#endif // BUDDIES_LIST_VIEW_DELEGATE_CONFIGURATION_H
