/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QCursor>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QMenu>
#include <QtWidgets/QTabBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>
#include <injeqt/injeqt.h>

#include "gui/widgets/custom-input.h"

#include "os/generic/compositing-aware-object.h"

#include "debug.h"

class Application;
class ChatWidgetManager;
class ChatWidget;
class Configuration;
class IconsManager;
class PluginInjectedFactory;
class OpenChatWithService;
class RecentChatsMenu;
class TabsManager;

class TabWidget : public QTabWidget, CompositingAwareObject
{
	Q_OBJECT

	QPointer<Application> m_application;
	QPointer<ChatWidgetManager> m_chatWidgetManager;
	QPointer<Configuration> m_configuration;
	QPointer<IconsManager> m_iconsManager;
	QPointer<PluginInjectedFactory> m_pluginInjectedFactory;
	QPointer<OpenChatWithService> m_openChatWithService;

	QToolButton *CloseChatButton;
	QToolButton *TabsListButton;
	QWidget *OpenChatButtonsWidget;
	QWidget *RightCornerWidget;
	RecentChatsMenu *RecentChatsMenuWidget;
	QMenu *TabsMenu;
	QToolButton *OpenRecentChatButton;

	TabsManager *Manager;

	bool config_oldStyleClosing;

	void updateTabsMenu();
	void updateTabsListButton();
	bool isTabVisible(int index);

private slots:
	INJEQT_SET void setApplication(Application *application);
	INJEQT_SET void setChatWidgetManager(ChatWidgetManager *chatWidgetManager);
	INJEQT_SET void setConfiguration(Configuration *configuration);
	INJEQT_SET void setIconsManager(IconsManager *iconsManager);
	INJEQT_SET void setPluginInjectedFactory(PluginInjectedFactory *pluginInjectedFactory);
	INJEQT_SET void setOpenChatWithService(OpenChatWithService *openChatWithService);
	INJEQT_INIT void init();

	void onContextMenu(int id, const QPoint &pos);

	/**
	* Slot zostaje wywołany w celu zmiany pozycji karty.
	* @param from pozycja karty źródłowej.
	* @param to pozycja karty docelowej.
	*/
	void moveTab(int from, int to);

	/**
	* Slot zostaje wywołany w chwili naciśnięcia przycisku
	* zamknięcia na karcie lub użycia środkowego przycisku myszy
	* @param numer karty, która ma zostać zamknięta.
	*/
	void onDeleteTab(int);

	/**
	* Slot zostaje wywołany w chwili naciśnięcia przycisku
	* otwracia nowej karty.
	* Tu otwiera okienko lub przywraca ja na pierwszy plan
	*/
	void newChat();

	/**
	* Slot that handles click on button with last conversations
	* Opens a popup QMenu with last conversations
	*/
	void openRecentChatsMenu();

	/**
	* Slot for handling click on item from popup QMenu with last conversations
	*/
	void openRecentChat(QAction *action);

	void deleteTab();

	void openTabsList();
	void currentTabChanged(int index);
	void tabsMenuSelected(QAction *action);

	void closeTab(ChatWidget *chatWidget);

protected:
	/**
	* Metoda wywoływana w chwili naciśnięcia x na ?pasku dekoracji?
	* Odpowiada za zamykanie wszystkich kart, lub tylko aktywnej karty
	* w zależności od konfiguracji
	*/
	virtual void closeEvent(QCloseEvent *e);
    virtual void showEvent(QShowEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
	/**
	* Metoda wywoływana w chwili przeciągnięcia obiektu na pasek kart
	* metodą dnd
	* akceptuje zdarzenie jeśli pochodziło z userbox'a lub z tabbara
	* @param e wskaźnik obiektu opisującego to zdarzenie.
	*/
	virtual void dragEnterEvent(QDragEnterEvent *e);

	/**
	* Metoda wywoływana w chwili upuszczenia przeciągniętego obiektu
	* na pasek kart
	* Wywołuje mętodę odpowiedzialną za przeniesienie karty lub otwarcie
	* nowej w zależnośći od zdarzenia
	* @param e wskaźnik obiektu opisującego to zdarzenie.
	*/
	virtual void dropEvent(QDropEvent *event);

	virtual void changeEvent(QEvent *event);

	virtual void tabInserted(int index);
	virtual void tabRemoved(int index);

	virtual void compositingEnabled();
	virtual void compositingDisabled();

public:
	explicit TabWidget(TabsManager *manager);
	virtual ~TabWidget();

	/**
	* Zmienia aktualną kartę na tą po lewej stronie
	*/
	void switchTabLeft();

	/**
	* Zmienia aktualną kartę na tą po prawej stronie
	*/
	void switchTabRight();

	void moveTabLeft();
	void moveTabRight();

	/**
	* Metoda wywoływana w momencie zaakceptowania zmian
	* w oknie konfiguracji kadu. Ustawua opcje modułu zgodnie
	* z konfiguracją
	*/
	void configurationUpdated();

	bool isChatWidgetActive(const ChatWidget *chatWidget);
	void tryActivateChatWidget(ChatWidget *chatWidget);
	void tryMinimizeChatWidget(ChatWidget *chatWidget);

public slots:
	/**
	* Slot zostaje wywołany w momencie wprowadzenia znaku w polu edycji
	* okna chat. Odpowiada za skórty klawiaturowe modułu, Jeśli handled
	* zostanie ustawione na true, skrót został przyęjty i wykonany
	* @param e wskaźnik obiektu opisującego to zdarzenie.
	* @param k wskaźnik do pola edycji z którego zostało wywołane zdarzenie.
	*/
	virtual void chatKeyPressed(QKeyEvent *e, CustomInput *k, bool &handled);

	/**
	* Slot zostaje wywołany w momencie dwukliku myszy
	* Tu powoduje otwarcie okna openChatWith;
	* @param e wskaźnik obiektu opisującego to zdarzenie.
	*/
	virtual void mouseDoubleClickEvent(QMouseEvent *e);

signals:
	void chatWidgetActivated(ChatWidget *chatWidget);
	void contextMenu(QWidget *w, const QPoint &pos);
	void openTab(QStringList altnicks, int index);

};
