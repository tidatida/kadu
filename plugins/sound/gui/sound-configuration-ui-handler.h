/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/gui/configuration-ui-handler.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ConfigComboBox;
class PluginInjectedFactory;
class NotifierConfigurationWidget;
class PathListEdit;
class SoundConfigurationWidget;
class SoundManager;
class SoundThemeManager;

class SoundConfigurationUiHandler : public QObject, public ConfigurationUiHandler
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit SoundConfigurationUiHandler(QObject *parent = nullptr);
	virtual ~SoundConfigurationUiHandler();

	NotifierConfigurationWidget * createConfigurationWidget(QWidget *parent = nullptr);

protected:
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;
	virtual void mainConfigurationWindowDestroyed() override;
	virtual void mainConfigurationWindowApplied() override;

private:
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
	QPointer<SoundManager> m_soundManager;
	QPointer<SoundThemeManager> m_soundThemeManager;

	QPointer<SoundConfigurationWidget> m_configurationWidget;
	QPointer<ConfigComboBox> m_themesComboBox;
	QPointer<PathListEdit> m_themesPaths;

	void connectWidgets();
	void setSoundThemes();

private slots:
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_SET void setSoundManager(SoundManager *soundManager);
	INJEQT_SET void setSoundThemeManager(SoundThemeManager *soundThemeManager);

	void themeChanged(const QString &theme);
	void soundFileEdited();

};
