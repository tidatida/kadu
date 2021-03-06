/*
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2005, 2006, 2007 Marcin Ślusarz (joi@kadu.net)
 * Copyright 2002, 2003, 2004, 2006 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2003, 2004 Tomasz Chiliński (chilek@chilan.com)
 * Copyright 2007, 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2004, 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2003, 2005 Paweł Płuciennik (pawel_p@kadu.net)
 * Copyright 2003, 2004 Dariusz Jagodzik (mast3r@kadu.net)
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef EMOTICONS_SELECTOR_BUTTON_H
#define EMOTICONS_SELECTOR_BUTTON_H

#include <QtWidgets/QLabel>

#include "emoticon.h"

class EmoticonPathProvider;

/**
 * @addtogroup Emoticons
 * @{
 */

/**
 * @class EmoticonSelectorButton
 * @short Small widget displaying static version of emoticon on an EmoticonSelector.
 */
class EmoticonSelectorButton : public QLabel
{
	Q_OBJECT

	Emoticon DisplayEmoticon;
	EmoticonPathProvider *PathProvider;

protected:
	void mouseMoveEvent(QMouseEvent *e);

public:
	/**
	 * @short Create new EmoticonSelectorButton widget.
	 * @param emoticon emoticon to display
	 * @param pathProvider EmoticonPathProvider used to get image file name for emoticon for popup widget
	 * @param parent parent widget
	 */
	explicit EmoticonSelectorButton(const Emoticon &emoticon, EmoticonPathProvider *pathProvider, QWidget *parent = nullptr);
	virtual ~EmoticonSelectorButton();

signals:
	/**
	 * @short Signal emited when button is clicked.
	 * @param emoticon clicked emoticon
	 */
	void clicked(const Emoticon &emoticon);

};

/**
 * @}
 */

#endif // EMOTICONS_SELECTOR_BUTTON_H
