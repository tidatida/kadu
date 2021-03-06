/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * %kadu copyright end%
 *
 * This file is derived from part of the KDE project
 * Copyright (C) 2007, 2006 Rafael Fernández López <ereslibre@kde.org>
 * Copyright (C) 2002-2003 Matthias Kretz <kretz@kde.org>
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

#include <QtCore/QPointer>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

class QLineEdit;
class QModelIndex;

class CategorizedListView;
class InjectedFactory;
class MainConfigurationWindow;
class PluginActivationService;
class PluginConflictResolver;
class PluginDependencyHandler;
class PluginModel;
class PluginProxyModel;
class PluginStateManager;
class PluginStateService;

/**
  * @short A widget to select what plugins to load and configure the plugins.
  *
  * It shows the list of available plugins
  *
  * @author Matthias Kretz <kretz@kde.org>
  * @author Rafael Fernández López <ereslibre@kde.org>
  */
class PluginListWidget : public QWidget
{
	Q_OBJECT

public:
	explicit PluginListWidget(MainConfigurationWindow *mainWindow);
	virtual ~PluginListWidget();

	void applyChanges();

	int dependantLayoutValue(int value, int width, int totalWidth) const;

private:
	friend class PluginListWidgetItemDelegate;
	friend class PluginModel;
	friend class PluginProxyModel;

	QPointer<InjectedFactory> m_injectedFactory;
	QPointer<PluginActivationService> m_pluginActivationService;
	QPointer<PluginConflictResolver> m_pluginConflictResolver;
	QPointer<PluginDependencyHandler> m_pluginDependencyHandler;
	QPointer<PluginStateManager> m_pluginStateManager;
	QPointer<PluginStateService> m_pluginStateService;

	CategorizedListView *m_listView;
	PluginModel *m_model;
	PluginProxyModel *m_proxyModel;
	bool m_processingChange;

	QSet<QString> activePluginsBeforeChange(const QString &changedPluginName, bool changedPluginChecked) const;
	QVector<QString> pluginsWithNewActiveState(bool newActiveState) const;

	void handleCheckedPlugin(const QString &pluginName, const QSet<QString> &modelActivePlugins);
	void handleUncheckedPlugin(const QString &pluginName, const QSet<QString> &modelActivePlugins);

	void setAllChecked(const QVector<QString> &plugins, bool checked);
	QString vectorToString(const QVector<QString> &plugins);

private slots:
	INJEQT_SET void setInjectedFactory(InjectedFactory *injectedFactory);
	INJEQT_SET void setPluginActivationService(PluginActivationService *pluginActivationService);
	INJEQT_SET void setPluginConflictResolver(PluginConflictResolver *pluginConflictResolver);
	INJEQT_SET void setPluginDependencyHandler(PluginDependencyHandler *pluginDependencyHandler);
	INJEQT_SET void setPluginStateManager(PluginStateManager *pluginStateManager);
	INJEQT_SET void setPluginStateService(PluginStateService *pluginStateService);
	INJEQT_INIT void init();

	void configurationApplied();
	void modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

};
