/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef DIALOG_WIDGET_H
#define DIALOG_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QDialog>

class QCheckBox;
class QLineEdit;
class QHBoxLayout;
class QLabel;
class QVBoxLayout;
class QFormLayout;
class TitleWidget;

class DialogWidget : public QWidget
{
	Q_OBJECT

protected:
	QString Title;
	QString WindowTitle;
	QPixmap Pixmap;

	virtual void createGui() = 0;

private slots:
	virtual void dialogAccepted() = 0;
	virtual void dialogRejected() = 0;

public:
	DialogWidget(const QString &windowTitle, const QString &title, const QPixmap &pixmap, QWidget* parent)
			: QWidget(parent), Title(title), WindowTitle(windowTitle), Pixmap(pixmap) {}
	virtual ~DialogWidget() {};

	QPixmap pixmap() { return Pixmap; }
	QString title() { return Title; }
	QString windowTitle() { return WindowTitle; }

signals:
	void valid(bool valid);
};

#endif // DIALOG_WIDGET_H
