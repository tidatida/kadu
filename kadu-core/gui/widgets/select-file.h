/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "exports.h"

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class IconsManager;

class QLineEdit;

class KADUAPI SelectFile : public QWidget
{
	Q_OBJECT

public:
	explicit SelectFile(const QString &type, QWidget *parent = nullptr);
	explicit SelectFile(QWidget *parent = nullptr);
	virtual ~SelectFile();

	void setType(const QString &type) { Type = type; }

	QString file() const;
	void setFile(const QString &file);

signals:
	void fileChanged();

private:
	QPointer<IconsManager> m_iconsManager;

	QString Type;
	QLineEdit *LineEdit;

	void createGui();

private slots:
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_INIT void init();

	void selectFileClicked();
	void fileEdited();

};
