/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GADU_WAIT_FOR_ACCOUNT_REGISTER_WINDOW_H
#define GADU_WAIT_FOR_ACCOUNT_REGISTER_WINDOW_H

#include <QtGui/QDialog>

#include "protocols/protocol.h"

class QLabel;

class GaduServerRegisterAccount;
class ProgressIcon;

class GaduWaitForAccountRegisterWindow : public QDialog
{
	Q_OBJECT

	bool CanClose;

	ProgressIcon *Progress;
	QLabel *MessageLabel;
	QPushButton *CloseButton;

	void createGui();
	void enableClosing();

private slots:
	void registerNewAccountFinished(GaduServerRegisterAccount *gsra);

protected:
	virtual void closeEvent(QCloseEvent *);

public:
	explicit GaduWaitForAccountRegisterWindow(GaduServerRegisterAccount *gsra, QWidget *parent = 0);
	virtual ~GaduWaitForAccountRegisterWindow();

signals:
	void uinRegistered(UinType);

};

#endif // GADU_WAIT_FOR_ACCOUNT_REGISTER_WINDOW_H
