/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "protocols/protocol.h"
#include "status/status-container.h"
#include "status_changer.h"

#include "status-button.h"

StatusButton::StatusButton(StatusContainer *statusContainer, QWidget *parent) :
		QPushButton(parent), MyStatusContainer(statusContainer)
{
	// statusChanged(MyStatusContainer->status(), MyStatusContainer->status());
	connect(MyStatusContainer, SIGNAL(statusChanged(Status, Status)), this, SLOT(statusChanged(Status, Status)));
}

void StatusButton::statusChanged(Status oldStatus, Status newStatus)
{
	setIcon(MyStatusContainer->statusPixmap());
	setText(MyStatusContainer->statusName());
}

void StatusButton::configurationUpdated()
{
	setIcon(MyStatusContainer->statusPixmap());
}
