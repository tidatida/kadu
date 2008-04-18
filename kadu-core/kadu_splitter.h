#ifndef KADU_SPLITTER_H
#define KADU_SPLITTER_H

#include <QSplitter>

class QChildEvent;

class KaduTextBrowser;

/**
	Klasa wykorzystana do oddzielenia listy użytkowników od historii
	i pola do wpisywania wiadomości
	\class KaduSplitter
	\brief Klasa rozdzielająca kontrolki

**/
class KaduSplitter : public QSplitter
{
	protected:
		QList<KaduTextBrowser*> textbrowsers;
// 		void drawContents(QPainter* p);
		void childEvent(QChildEvent* c);

	public:
	/**
		Konstruktor tworzący obiekt
		\fn KaduSplitter(QWidget* parent = 0, const char* name = 0)
		\param parent rodzic okna
		\param name nazwa obiektu

	**/
		KaduSplitter(QWidget* parent = 0, const char* name = 0);

	/**
		Konstruktor tworzący obiekt o danej orientacji
		\fn KaduSplitter(Orientation o, QWidget* parent = 0, const char* name = 0)
		\param parent rodzic okna
		\param name nazwa obiektu
		\param o orientacja separatora
	**/
		KaduSplitter(Qt::Orientation o, QWidget* parent = 0, const char* name = 0);
};

#endif
