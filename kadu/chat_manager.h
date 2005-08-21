#ifndef KADU_CHAT_MANAGER_H
#define KADU_CHAT_MANAGER_H

#include <qobject.h>

#include "chat.h"
#include "chat_message.h"
#include "usergroup.h"

class ChatManagerSlots;

/**
	Klasa pozwalaj�ca zarz�dza� otwartymi oknami rozm�w: otwiera�,
	zamykac, szuka� okna ze wgl�du na list� u�ytkownik�w itp.
	\class ChatManager
	\brief Klasa zarz�dzaj�ca oknami Chat
**/

class ChatManager : public QObject
{
	Q_OBJECT

	private:
		ChatList Chats;/*!< lista okien*/

		static ChatManagerSlots *chatslots;
		/**
		\fn int openPendingMsg(int index, ChatMessage &msg)
		Funkcja otwieraj�ca zakolejkowan� wiadomo��
		\param index nr wiadomo�ci z kolejki
		\param msg wiadomo�� z kolejki
		\return zwracany jest nr okna w kt�rym zosta�a otwarta wiadomo��
		**/
		int openPendingMsg(int index, ChatMessage &msg);

		/**
		\struct ChatInfo
		Struktura przechowuje informacje o danym oknie
		**/
		struct ChatInfo
		{
			const UserGroup *users;/*!< lista u�ytkownik�w
					identyfikuj�ca okno */
			QMap<QString, QVariant> map; /*!< parametry
							danego okna */
		};
		QValueList<ChatInfo> addons; /*!< lista parametr�w okien */

	public:
		/**
		\fn ChatManager(QObject* parent=NULL, const char* name=NULL)
		Konstruktor tworz�cy obiekt zarz�dzaj�cy oknami
		\param parent rodzic okna
		\param name nazwa obiektu
		**/
		ChatManager(QObject* parent=NULL, const char* name=NULL);

		/**
			\fn ~ChatManager()
			Destruktor zamyka wszystkie otwarte okna
		**/

		~ChatManager();

		/**
			\fn const ChatList& chats() const
			Funkcja zwraca liste otwartych okien Chat
		**/

		/**
			\fn static void initModule()
			Rejestruje opcje modulu Chat w oknie konfiguracji
		**/
		static void initModule();

		static void closeModule();

		const ChatList& chats() const;

		/**
			\fn Chat* findChat(const UserGroup *group) const;
			Funkcja zwraca wska�nik do okna z list�
			u�ytkownik�w group
			\param group lista u�ytkownik�w
			\return wska�nik do okna je�li istnieje w przeciwnym
			 wypadku zwraca NULL
		**/
		Chat* findChat(const UserGroup *group) const;

		/**
			\fn Chat* findChat(UserListElements users) const;
			Funkcja zwraca wska�nik do okna z list�
			u�ytkownik�w group
			\param users lista u�ytkownik�w
			\return wska�nik do okna je�li istnieje w przeciwnym
			 wypadku zwraca NULL
		**/
		Chat* findChat(UserListElements users) const;

		/**
			\fn QVariant& getChatProperty(const UserGroup *group, const QString &name)
			Funkcja zwraca warto�� w�asno�ci "name" okna
			okre�lonego przez group
			\param group grupa u�ytkownik�w identyfikuj�ca okno
			\param name nazwa w�asno�ci
			\return zwraca warto�� w�asno�ci je�li okre�lone okno
			istnieje,\n je�li nie to tworzy tak�
			w�asno�� (ustawia na pust�)
		**/
		QVariant& getChatProperty(const UserGroup *group, const QString &name);

	public slots:

		/**
			\fn void chatMsgReceived(const QString &protocolName, UserListElements senders, const QString& msg, time_t time, bool& grab)
			Slot informuj�cy o odebraniu wiadomo�ci
			\param protocolName nazwa protoko�u
			\param senders lista u�ytkownik�w identyfikuj�cych okno
			\param msg tre�� otrzymanej wiadomo�ci
			\param time czas otrzymania wiadomo�ci
			\param grab
		**/
		void chatMsgReceived(const QString &protocolName, UserListElements senders, const QString& msg, time_t time, bool& grab);

		/**
			\fn int openChat(QString initialProtocol, UserListElements users, time_t time = 0)
			Funkcja otwiera nowe okno Chat z wymienionymi rozmowcami.
			\param initialProtocol protok� pocz�tkowy
			\param users lista u�ytkownik�w identyfikuj�cych okno
			\param time parametr sluzy do sprawdzenia ile wiadomosci
			z historii ma sie pojawic w oknie.
			\return zwracany jest numer otwartego okna
		**/
		int openChat(QString initialProtocol, UserListElements users, time_t time = 0);

		/**
			\fn void openPendingMsgs(UserListElements users)
			Funkcja wpisuje zakolejkowane wiadomo�ci do okna
			z u�ytkownikami "users"
			\param users lista u�ytkownik�w identyfikuj�cych okno
		**/
		void openPendingMsgs(UserListElements users);

		/**
			\fn void openPendingMsgs()
			Funkcja wpisuje wszystkie zakolejkowane wiadomo�ci
			do odpowiednich okien
		**/
		void openPendingMsgs();

		/**
			\fn void deletePendingMsgs(UserListElements users)
			Funkcja usuwa zakolejkowane wiadomo�ci
			z u�ytkownikami "users"
			\param users lista u�ytkownik�w identyfikuj�cych okno
		**/
		void deletePendingMsgs(UserListElements users);


		//TODO: opisac funkcje sendMessage(..)
		/*
			Niebardzo rozumiem tej funkcji (czemu jest uin i uins)
		*/
		void sendMessage(UserListElement user, UserListElements selected_users);

		/**
			\fn void closeAllWindows()
			Funkcja zamyka wszystkie okna chat
		**/
		void closeAllWindows();

		/**
			\fn int registerChat(Chat* chat)
			Dodaje okno do menad�era
			\param chat wska�nik do okna ktore chcemy doda�
			\return zwraca numer naszego okna po zarejestrowaniu
		**/
		int registerChat(Chat* chat);

		/**
			\fn void unregisterChat(Chat* chat)
			Funkcja wyrejestrowuje okno z managera \n
			Zapisuje w�asno�ci okna \n
			wysy�a sygna� chatDestroying i chatDestroyed
			\param chat okno kt�re b�dzie wyrejestrowane
		**/
		void unregisterChat(Chat* chat);

		/**
			\fn void refreshTitles()
			Funkcja od�wie�a tytu�y wszystkich okien
		**/
		void refreshTitles();

		/**
			\fn void refreshTitlesForUser(UserListElement user)
			Funkcja od�wie�a tytu�y okien kt�re zawieraj� uin
			\param user u�ytkownik, kt�rego
			opis/status b�dzie od�wie�any
		**/
		void refreshTitlesForUser(UserListElement user);

		/**
			\fn void changeAppearance()
			Funkcja od�wie�a kolory i czcionki we wszystkich oknach
		**/
		void changeAppearance();

		/**
			\fn void setChatProperty(const UserGroup *group, const QString &name, const QVariant &value)
			Funkcja pozwala przypisa� okre�lonemu czatowi
			(nawet je�eli on jeszcze nie istnieje) pewne w�asno�ci
			\param group grupa u�ytkownik�w identyfikuj�cych okno
			\param name nazwa w�asno�ci
			\param value warto�� w�asno�ci
		**/
		void setChatProperty(const UserGroup *group, const QString &name, const QVariant &value);

	signals:

		/**
			\fn void chatCreated(const UserGroup *group)
		 	Sygna� ten jest wysy�any po utworzeniu nowego okna chat
			\param group lista u�ytkownik�w, z kt�rymi tworzymy nowy chat
		**/
		void chatCreated(const UserGroup *group);

		/**
			\fn void chatDestroying(const UserGroup *group)
		 	Sygna� ten jest wysy�any przed zamnkni�ciem okna chat
			\param group lista u�ytkownik�w�w, kt�rzy wyst�puj� w tym chacie
		**/
		void chatDestroying(const UserGroup *group);

		/**
			\fn void chatDestroyed(const UserGroup *group)
		 	Sygna� ten jest wysy�any po zamnkni�ciem okna chat
			\param group lista u�ytkownik�w, kt�rzy
			wyst�powa�y w tym chacie
		**/
		void chatDestroyed(const UserGroup *group);

		/**
			\fn void chatOpen(UserListElements users)
			Sygna� ten jest wysy�aniy podczas ka�dej pr�by
			otwarcia nowego okna chat nawet je�li ju� taki istnieje
			\param users lista u�ytkownik�w
		**/
		void chatOpen(UserListElements users);
};

/**
	wska�nik do obiektu ChatManager'a
**/
extern ChatManager* chat_manager;

#endif
