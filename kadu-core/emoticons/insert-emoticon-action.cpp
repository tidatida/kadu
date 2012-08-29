/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/configuration-file.h"
#include "emoticons/emoticon-selector.h"
#include "gui/actions/action.h"
#include "gui/widgets/chat-edit-box.h"

#include "insert-emoticon-action.h"

InsertEmoticonAction::InsertEmoticonAction(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeChat);
	setName("insertEmoticonAction");
	setIcon(KaduIcon("face-smile"));
	setText(tr("Insert Emoticon"));

	registerAction();
}

InsertEmoticonAction::~InsertEmoticonAction()
{
}

void InsertEmoticonAction::actionInstanceCreated(Action *action)
{
	ActionDescription::actionInstanceCreated(action);

	if (config_file.readEntry("Chat","EmoticonsTheme").trimmed().isEmpty())
	{
		action->setToolTip(tr("Insert emoticon - enable in configuration"));
		action->setEnabled(false);
	}

}

void InsertEmoticonAction::configurationUpdated()
{
	ActionDescription::configurationUpdated();

	QString toolTip;
	bool enabled;

	if (config_file.readEntry("Chat","EmoticonsTheme").trimmed().isEmpty())
	{
		toolTip =  tr("Insert emoticon - enable in configuration");
		enabled = false;
	}
	else
	{
		toolTip = tr("Insert emoticon");
		enabled = true;
	}

 	foreach (Action *action, actions())
	{
		action->setToolTip(toolTip);
		action->setEnabled(enabled);
	}
}

void InsertEmoticonAction::actionTriggered(QAction *sender, bool toggled)
{
	Q_UNUSED(toggled)

	ChatEditBox *chatEditBox = qobject_cast<ChatEditBox *>(sender->parent());
	if (!chatEditBox)
		return;

	QList<QWidget *> widgets = sender->associatedWidgets();
	if (widgets.isEmpty())
		return;

	EmoticonSelector *emoticonSelector = new EmoticonSelector(widgets.at(widgets.size() - 1));
	connect(emoticonSelector, SIGNAL(emoticonSelect(const QString &)), chatEditBox, SLOT(insertPlainText(QString)));
	emoticonSelector->show();
}
