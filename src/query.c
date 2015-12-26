/*
 * query.c -- querying the user for input
 *             
 *
 * Copyright (C) 2001,2003 Øyvind Kolås <pippin@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <string.h>
#include "tree.h"
#include "prefs.h"
#include "ui.h"
#include "libcli/cli.h"

static char query[100];

static long getquery_cmd (int argc, char **argv, long *data)
{
	Node *pos = (Node *) data;

	strcpy (query, "");
	if(argc==2)
		ui_getstr (argv[1], &query[0]);
	else
		ui_getstr ("enter string", &query[0]);
	return (long) pos;
}

/*
!init_query();
*/
void init_query ()
{
	cli_add_command ("getquery", getquery_cmd, "<prompt>");
	cli_add_help ("getquery",
				  "Input a string from the user (put into the variable query, used by amongst other function the search function, and at least some of the export/import functions");
	cli_add_string ("query", query,
					"last query (also settable as a variable)");
}
