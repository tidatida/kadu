/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "configuration/chat-configuration-holder.h"
#include "dom/dom-processor-service.h"
#include "emoticons/emoticons-manager.h"
#include "formatted-string/formatted-string.h"
#include "formatted-string/formatted-string-dom-visitor.h"
#include "message/message.h"

#include "message-html-renderer-service.h"

MessageHtmlRendererService::MessageHtmlRendererService(QObject *parent) :
		QObject(parent)
{
}

MessageHtmlRendererService::~MessageHtmlRendererService()
{
}

void MessageHtmlRendererService::setDomProcessorService(DomProcessorService *domProcessorService)
{
	CurrentDomProcessorService = domProcessorService;
}

QString MessageHtmlRendererService::renderMessage(const Message &message)
{
	FormattedStringDomVisitor formattedStringDomVisitor;
	message.content()->accept(&formattedStringDomVisitor);

	QDomDocument domDocument = formattedStringDomVisitor.result();

	if (CurrentDomProcessorService)
		CurrentDomProcessorService.data()->process(domDocument);

	if (domDocument.documentElement().childNodes().isEmpty())
		return QString();

	QString result = domDocument.toString(0).trimmed();
	// remove <message></message>
	Q_ASSERT(result.startsWith(QLatin1String("<message>")));
	Q_ASSERT(result.endsWith(QLatin1String("</message>")));
	return result.mid(qstrlen("<message>"), result.length() - qstrlen("<message></message>"));
}
