/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"

#include "id-regular-expression-filter.h"

IdRegularExpressionFilter::IdRegularExpressionFilter(QObject *parent) :
		AbstractAccountFilter(parent)
{
}

IdRegularExpressionFilter::~IdRegularExpressionFilter()
{
}

void IdRegularExpressionFilter::setId(QString id)
{
	if (Id == id)
		return;

	Id = id;
	emit filterChanged();
}

bool IdRegularExpressionFilter::acceptAccount(Account account)
{
	if (Id.isEmpty())
		return true;
	
	QRegExp regularExpression = account.protocolHandler()->protocolFactory()->idRegularExpression();
	if (regularExpression.exactMatch(Id))
		return true;

	return regularExpression.matchedLength() == Id.length(); // for not-full matches
}
