/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CONFIG_PROXY_COMBO_BOX_H
#define CONFIG_PROXY_COMBO_BOX_H

#include "gui/widgets/proxy-combo-box.h"
#include "gui/widgets/configuration/config-widget-value.h"

class QLabel;
class QDomElement;
class ConfigGroupBox;

class KADUAPI ConfigProxyComboBox : public ProxyComboBox, public ConfigWidgetValue
{
	Q_OBJECT

	QLabel *Label;
	QString DefaultItem;
	bool ShowDefault;

protected:
	virtual void createWidgets();

public:
	ConfigProxyComboBox(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigProxyComboBox();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);

};

#endif // CONFIG_PROXY_COMBO_BOX_H
