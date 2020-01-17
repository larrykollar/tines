/* libcli -- a small commandline interpreter libraray
 * Copyright (C) 2002 Øyvind Kolås
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <string.h>

#define HISTORY_SIZE 64
#define MAXLEN 100

static char history[HISTORY_SIZE][MAXLEN] = { {0} };

static unsigned int readpos = 0;
static unsigned int writepos = 0;
static unsigned int skiptwoforward = 0;

static void init (void)
{
	static int done = 0;

	if (!done) {
		done = 1;
		memset (history, 0, sizeof (history));
	}
	readpos %= HISTORY_SIZE;
	writepos %= HISTORY_SIZE;
}

void cli_historyadd (char *string)
{
	init ();
	if (!string[0])
		return;					/* don't bother with empty strings */
	strncpy (history[writepos], string, MAXLEN - 1);
	history[writepos][99] = 0;

	readpos = writepos;

	writepos++;
	readpos %= HISTORY_SIZE;
	writepos %= HISTORY_SIZE;
	history[writepos][0] = 0;	/* for å hindre wraparound i tid */
}

char *cli_historyprev ()
{
	char *ret;

	init ();
	ret = history[readpos];
	if (!ret[0])
		return "";				/* nothing there */
	readpos--;

	readpos %= HISTORY_SIZE;
	writepos %= HISTORY_SIZE;

	skiptwoforward = 1;
	return ret;
}

char *cli_historynext ()
{
	char *ret;

	readpos++;
	if (skiptwoforward)
		readpos++;
	init ();
	ret = history[readpos];

	if (!ret[0])
		readpos = writepos - 1;

	readpos %= HISTORY_SIZE;
	writepos %= HISTORY_SIZE;

	skiptwoforward = 0;
	return ret;
}
