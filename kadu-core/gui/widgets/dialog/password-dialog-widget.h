/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "gui/widgets/dialog/dialog-widget.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QVariant>
#include <injeqt/injeqt.h>

class IconsManager;

class QCheckBox;
class QLineEdit;
class QFormLayout;

class PasswordDialogWidget : public DialogWidget
{
	Q_OBJECT

public:
	explicit PasswordDialogWidget(const QString &title, QVariant data, QWidget* parent);
	virtual ~PasswordDialogWidget();

signals:
	void passwordEntered(const QVariant &data, const QString &password, bool permanent);

private:
	QPointer<IconsManager> m_iconsManager;
	QVariant Data;

	QFormLayout *formLayout;
	QLineEdit *Password;
	QCheckBox *Store;

	virtual void createGui();

private slots:
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_INIT void init();

	virtual void dialogAccepted();
	virtual void dialogRejected();
	void passwordTextChanged(const QString &text);

};
