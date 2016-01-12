/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QSortFilterProxyModel>
#include <QtWidgets/QAction>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QLineEdit>

#include "buddies/group-manager.h"
#include "buddies/model/groups-model.h"
#include "core/core.h"
#include "model/model-chain.h"
#include "model/roles.h"

#include "groups-combo-box.h"

GroupsComboBox::GroupsComboBox(QWidget *parent) :
		ActionsComboBox(parent)
{
	addBeforeAction(new QAction(tr(" - Do not add - "), this));

	CreateNewGroupAction = new QAction(tr("Create a new group..."), this);
	QFont createNewGroupActionFont = CreateNewGroupAction->font();
	createNewGroupActionFont.setItalic(true);
	CreateNewGroupAction->setFont(createNewGroupActionFont);
	CreateNewGroupAction->setData(true);
	connect(CreateNewGroupAction, SIGNAL(triggered()), this, SLOT(createNewGroup()));
	addAfterAction(CreateNewGroupAction);

	ModelChain *chain = new ModelChain(this);
	chain->setBaseModel(new GroupsModel(Core::instance()->groupManager(), chain));
	QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(this);
	chain->addProxyModel(proxyModel);
	setUpModel(GroupRole, chain);
	proxyModel->setDynamicSortFilter(true);
	proxyModel->sort(0);

	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

GroupsComboBox::~GroupsComboBox()
{
}

void GroupsComboBox::setCurrentGroup(Group group)
{
	setCurrentValue(group);
}

Group GroupsComboBox::currentGroup()
{
	return currentValue().value<Group>();
}

void GroupsComboBox::createNewGroup()
{
	bool ok;

	QString newGroupName = QInputDialog::getText(this, tr("New Group"),
			tr("Please enter the name for the new group:"), QLineEdit::Normal,
			QString(), &ok);

	if (!ok)
		return;

	ok = Core::instance()->groupManager()->acceptableGroupName(newGroupName, true);
	if (!ok)
		return;

	Group newGroup = Core::instance()->groupManager()->byName(newGroupName, ok);
	if (newGroup)
		setCurrentGroup(newGroup);
}

#include "moc_groups-combo-box.cpp"
