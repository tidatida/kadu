/* $Id: search50.c,v 1.1 2003/01/12 22:59:09 chilek Exp $ */

/*
 *  (C) Copyright 2003 Wojtek Kaniewski <wojtekka@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License Version
 *  2.1 as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>
#include <errno.h>
#ifndef _AIX
#  include <string.h>
#endif
#include "compat.h"
#include "libgadu.h"

/*
 * gg_search50_new()
 *
 * tworzy now� zmienn� typu gg_search50_t.
 *
 * zaalokowana zmienna lub NULL w przypadku braku pami�ci.
 */
gg_search50_t gg_search50_new()
{
	gg_search50_t res = malloc(sizeof(struct gg_search50_s));

	gg_debug(GG_DEBUG_FUNCTION, "** gg_search50_new();\n");

	if (!res) {
		gg_debug(GG_DEBUG_MISC, "// gg_search50_new() out of memory\n");
		return NULL;
	}

	memset(res, 0, sizeof(struct gg_search50_s));

	return res;
}

/*
 * gg_search50_add_n()  // funkcja wewn�trzna
 *
 * funkcja dodaje pole do zapytania lub odpowiedzi.
 *
 *  - req - wska�nik opisu zapytania,
 *  - num - numer wyniku (0 dla zapytania),
 *  - field - nazwa pola,
 *  - value - warto�� pola,
 *
 * 0/-1
 */
int gg_search50_add_n(gg_search50_t req, int num, const char *field, const char *value)
{
	struct gg_search50_entry *tmp = NULL, *entry;
	char *dupfield, *dupvalue;

	gg_debug(GG_DEBUG_FUNCTION, "** gg_search50_add_n(%p, %d, \"%s\", \"%s\");\n", req, num, field, value);

	if (!(dupfield = strdup(field))) {
		gg_debug(GG_DEBUG_MISC, "// gg_search50_add_n() out of memory\n");
		return -1;
	}

	if (!(dupvalue = strdup(value))) {
		gg_debug(GG_DEBUG_MISC, "// gg_search50_add_n() out of memory\n");
		free(dupfield);
		return -1;
	}

	if (!(tmp = realloc(req->entries, sizeof(struct gg_search50_entry) * (req->entries_count + 1)))) {
		gg_debug(GG_DEBUG_MISC, "// gg_search50_add_n() out of memory\n");
		free(dupfield);
		free(dupvalue);
		return -1;
	}

	req->entries = tmp;

	entry = &req->entries[req->entries_count];
	entry->num = num;
	entry->field = dupfield;
	entry->value = dupvalue;

	req->entries_count++;

	return 0;
}

/*
 * gg_search50_add()
 *
 * funkcja dodaje pole do zapytania.
 *
 *  - req - wska�nik opisu zapytania,
 *  - field - nazwa pola,
 *  - value - warto�� pola,
 *
 * 0/-1
 */
int gg_search50_add(gg_search50_t req, const char *field, const char *value)
{
	return gg_search50_add_n(req, 0, field, value);
}

/*
 * gg_search50_free()
 *
 * zwalnia pami�� po zapytaniu lub rezultacie szukania u�ytkownika.
 *
 *  - s - zwalniana zmienna,
 */
void gg_search50_free(gg_search50_t s)
{
	int i;

	if (!s)
		return;
	
	for (i = 0; i < s->entries_count; i++) {
		free(s->entries[i].field);
		free(s->entries[i].value);
	}

	free(s->entries);
}

/*
 * gg_search50()
 *
 * wysy�a zapytanie katalogu publicznego do serwera.
 *
 *  - sess - sesja,
 *  - req - zapytanie.
 *
 * numer sekwencyjny wyszukiwania lub 0 w przypadku b��du.
 */
uint32_t gg_search50(struct gg_session *sess, gg_search50_t req)
{
	int i, size = 5;
	uint32_t res;
	char *buf, *p;
	struct gg_search50_request *r;

	gg_debug(GG_DEBUG_FUNCTION, "** gg_search50(%p, %p);\n", sess, req);
	
	if (!sess || !req) {
		gg_debug(GG_DEBUG_MISC, "// gg_search50() invalid arguments\n");
		errno = EFAULT;
		return 0;
	}

	if (sess->state != GG_STATE_CONNECTED) {
		gg_debug(GG_DEBUG_MISC, "// gg_search50() not connected\n");
		errno = ENOTCONN;
		return 0;
	}

	for (i = 0; i < req->entries_count; i++) {
		/* wyszukiwanie bierze tylko pierwszy wpis */
		if (req->entries[i].num)
			continue;
		
		size += strlen(req->entries[i].field) + 1;
		size += strlen(req->entries[i].value) + 1;
	}

	if (!(buf = malloc(size))) {
		gg_debug(GG_DEBUG_MISC, "// gg_search50() out of memory (%d bytes)\n", size);
		return 0;
	}

	r = (struct gg_search50_request*) buf;
	res = (random() & 0xffffff00);
	r->dunno1 = gg_fix32(res | 0x03);
	r->dunno2 = '>';

	for (i = 0, p = buf + 5; i < req->entries_count; i++) {
		if (req->entries[i].num)
			continue;

		strcpy(p, req->entries[i].field);
		p += strlen(p) + 1;

		strcpy(p, req->entries[i].value);
		p += strlen(p) + 1;
	}

	if (gg_send_packet(sess->fd, GG_SEARCH50_REQUEST, buf, size, NULL, 0) == -1)
		res = 0;

	free(buf);

	return res;
}

/*
 * gg_search50_handle_reply()  // funkcja wewn�trzna
 *
 * analizuje przychodz�cy pakiet odpowiedzi i zapisuje wynik wyszukiwania
 * w struct gg_event.
 *
 *  - e - opis zdarzenia,
 *  - packet - zawarto�� pakietu odpowiedzi,
 *  - length - d�ugo�� pakietu odpowiedzi.
 *
 * 0/-1
 */
int gg_search50_handle_reply(struct gg_event *e, const char *packet, int length)
{
	const char *end = packet + length, *p;
	struct gg_search50_reply *r;
	gg_search50_t res;
	int num = 0;
	
	gg_debug(GG_DEBUG_FUNCTION, "** gg_search50_handle_reply(%p, %p, %d);\n", e, packet, length);

	if (!e || !packet) {
		gg_debug(GG_DEBUG_MISC, "// gg_search50_handle_reply() invalid arguments\n");
		errno = EINVAL;
		return -1;
	}

	if (length < 5) {
		gg_debug(GG_DEBUG_MISC, "// gg_search50_handle_reply() packet too short\n");
		errno = EINVAL;
		return -1;
	}

	if (!(res = gg_search50_new())) {
		gg_debug(GG_DEBUG_MISC, "// gg_search50_handle_reply() unable to allocate reply\n");
		return -1;
	}

	e->event.search50 = res;

	r = (struct gg_search50_reply*) packet;

	res->seq = (fix32(r->dunno1) & 0xffffff00);

	/* pomi� pocz�tek odpowiedzi */
	p = packet + 5;

	while (p < end) {
		const char *field, *value;

		field = p;

		/* sprawd�, czy nie mamy podzia�u na kolejne pole */
		if (!*field) {
			num++;
			field++;
		}

		value = NULL;
		
		for (p = field; p < end; p++) {
			/* je�li mamy koniec tekstu... */
			if (!*p) {
				/* ...i jeszcze nie mieli�my warto�ci pola to
				 * wiemy, �e po tym zerze jest warto��... */
				if (!value)
					value = p + 1;
				else
					/* ...w przeciwym wypadku koniec
					 * warto�ci i mo�emy wychodzi�
					 * grzecznie z p�tli */
					break;
			}
		}
		
		/* sprawd�my, czy pole nie wychodzi poza pakiet, �eby nie
		 * mie� segfault�w, je�li serwer przestanie zaka�cza� pakiet�w
		 * przez \0 */

		if (p == end) {
			gg_debug(GG_DEBUG_MISC, "// gg_search50_handle_reply() premature end of packet\n");
			goto failure;
		}

		p++;

		/* je�li dostali�my namier na nast�pne wyniki, to znaczy �e
		 * mamy koniec wynik�w i nie jest to kolejna osoba. */
		if (!strcasecmp(field, "nextstart")) {
			res->next = atoi(value);
			num--;
		} else {
			if (gg_search50_add_n(res, num, field, value) == -1)
				goto failure;
		}
	}	

	res->count = num + 1;
	
	return 0;

failure:
	gg_search50_free(res);
	return -1;
}

/*
 * gg_search50_get()
 *
 * pobiera informacj� z rezultatu wyszukiwania.
 *
 *  - res - rezultat wyszukiwania,
 *  - num - numer odpowiedzi,
 *  - field - nazwa pola (wielko�� liter nie ma znaczenia).
 *
 * warto�� pola lub NULL, je�li nie znaleziono.
 */
const char *gg_search50_get(gg_search50_t res, int num, const char *field)
{
	char *value = NULL;
	int i;

	gg_debug(GG_DEBUG_FUNCTION, "** gg_search50_field(%p, %d, \"%s\");\n", res, num, field);

	if (!res || num < 0 || !field) {
		gg_debug(GG_DEBUG_MISC, "// gg_search50_field() invalid arguments\n");
		errno = EINVAL;
		return NULL;
	}

	for (i = 0; i < res->entries_count; i++) {
		if (res->entries[i].num == num && !strcasecmp(res->entries[i].field, field)) {
			value = res->entries[i].value;
			break;
		}
	}

	return value;
}

/*
 * gg_search50_count()
 *
 * zwraca ilo�� znalezionych os�b.
 *
 *  - res - wynik szukania.
 *
 * ilo�� lub -1 w przypadku b��du.
 */
int gg_search50_count(gg_search50_t res)
{
	return (!res) ? -1 : res->count;
}

/*
 * gg_search50_next()
 *
 * zwraca numer, od kt�rego nale�y rozpocz�� kolejne wyszukiwanie, je�li
 * zale�y nam na kolejnych wynikach.
 *
 *  - res - wynik szukania.
 *
 * numer lub -1 w przypadku b��du.
 */
uin_t gg_search50_next(gg_search50_t res)
{
	return (!res) ? -1 : res->next;
}

/*
 * gg_search50_seq()
 *
 * zwraca numer sekwencyjny wyszukiwania.
 *
 *  - res - wynik szukania.
 *
 * numer lub -1 w przypadku b��du.
 */
uint32_t gg_search50_seq(gg_search50_t res)
{
	return (!res) ? -1 : res->seq;
}

/*
 * Local variables:
 * c-indentation-style: k&r
 * c-basic-offset: 8
 * indent-tabs-mode: notnil
 * End:
 *
 * vim: shiftwidth=8:
 */
