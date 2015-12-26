/*
 * prefs.c -- preferences and global variable mangement of hnb
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
#define PREFS_C

#include <stdlib.h>
#include <string.h>
#include <curses.h>

#include "tree.h"
#include "ui.h"
#include "path.h"
#include "prefs.h"
#include "cli.h"
#include "ui_cli.h"
#include "file.h"


Tprefs prefs = {
	"hnb",					/*format; */
	0,							/*tutorial; */
	"",							/*rc_file[100]; */
	"",							/*db_file[100]; */
	"",							/*default_db_file[100]; */
	1,							/*showpercent */
	0,							/*fixed focusbar */
	0,							/*save position */
	0					/*readonly */
};

typedef struct {
	char name[4];
	int color;
} ColornameT;

/*
!init_prefs();
*/
void init_prefs ()
{
#ifndef WIN32
	sprintf (prefs.rc_file, "%s/.hnbrc", getenv ("HOME"));
	sprintf (prefs.default_db_file, "%s/.hnb", getenv ("HOME"));
#endif
#ifdef WIN32
	sprintf (prefs.rc_file, "C:\\hnb.rc");
	sprintf (prefs.default_db_file, "C:\\hnb_data");
#endif
	
	cli_add_string ("format", prefs.format, "the format of the current file");

	cli_add_string ("rc_file", prefs.rc_file, "");
	cli_add_string ("db_file", prefs.db_file, "");
	cli_add_string ("default_db_file", prefs.default_db_file, "");

	cli_add_int ("showpercent", &prefs.showpercent, "");
	cli_add_int ("fixedfocus", &prefs.fixedfocus, "");
	cli_add_int ("savepos", &prefs.savepos, "");
	cli_add_int ("readonly", &prefs.readonly, "");

#ifdef NCURSES_VERSION 
	cli_add_int ("escdelay", (long *)&ESCDELAY,
				 "how long does curses wait before it decides ESC is ESC and not a coded key sequence");
#endif
}

void write_default_prefs ()
{
	FILE *file;

	file = fopen (prefs.rc_file, "w");
	fprintf (file,
#include "hnbrc.inc"
		);
	fclose (file);
}

void load_prefs (void)
{
	if (xml_check (prefs.rc_file)) {
		printf
			("seems like your current ~/.hnbrc is outdated (it's xml the new format\n\
is plain text,.. remove it and let hnb make a new default\n");
		exit (0);
	}
	cli_load_file (prefs.rc_file);
}
