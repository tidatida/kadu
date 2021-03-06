/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#pragma once

#include "exports.h"

class QDomDocument;
class QDomNode;

class DomVisitor;

/**
 * @addtogroup Dom
 * @{
 */

/**
 * @class DomProcessor
 * @short Helper class for processing DOM documents.
 * @author Rafał 'Vogel' Malinowski
 *
 * This class can be used to process DOM documents using visitor pattern.
 * Visitor object is allowed to modify DOM tree in any way.
 */
class KADUAPI DomProcessor
{
	
public:
	/**
	 * @short Create new instance of DomProcessor.
	 * @author Rafał 'Vogel' Malinowski
	 * @param domDocument DOM document to process
	 */
	DomProcessor(QDomDocument &domDocument);

	/**
	 * @short Accept a visitor.
	 * @author Rafał 'Vogel' Malinowski
	 * @param visitor to accept
	 *
	 * Processing is not read-only, so domDocument can be changed after calling this method.
	 */
	void accept(const DomVisitor *visitor);

	/**
	 * @short Accept set of visitors.
	 * @author Rafał 'Vogel' Malinowski
	 * @param visitor to accept
	 *
	 * Processing is not read-only, so domDocument can be changed after calling this method.
	 */
	template<class Iterator>
	void accept(Iterator begin, Iterator end)
	{
		std::for_each(begin, end, [this](const DomVisitor *visitor){ accept(visitor); });
	}

private:
	QDomDocument &m_domDocument;

	QDomNode acceptNode(const DomVisitor *visitor, QDomNode node);

};

class invalid_xml : public std::exception {};

KADUAPI QDomDocument toDomDocument(const QString &xml);
KADUAPI QString toString(const QDomDocument &domDocument);
KADUAPI QString processDom(const QString &xml, const DomVisitor &domVisitor);

/**
 * @}
 */
