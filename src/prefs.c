/*
 * prefs.c -- preferences and global variable mangement of tines
 *
 * Copyright (C) 2001,2003 Øyvind Kolås <pippin@users.sourceforge.net>
 * modified for tines project by Larry Kollar (lkollar@gmail.com)
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
#include <unistd.h>

#include "config.h"
#include "tree.h"
#include "ui.h"
#include "path.h"
#include "prefs.h"
#include "cli.h"
#include "ui_cli.h"
#include "file.h"
#include "file_copy.h"


Tprefs prefs = {
	"hnb",					/* format; */
	0,						/* tutorial; */
	"",						/* rc_file[100]; */
	"",						/* db_file[100]; */
	"",						/* default_db_file[100]; */
	1,						/* showpercent */
	0,						/* fixed focusbar */
	0,						/* save position */
	0,						/* save expanded state */
	0,						/* readonly */
	1						/* curses */
};

long pid;

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
	sprintf (prefs.rc_file, "%s/.tinesrc", getenv ("HOME"));
	sprintf (prefs.default_db_file, "%s/.tines", getenv ("HOME"));
#endif
#ifdef WIN32
	sprintf (prefs.rc_file, "C:\\tines.rc");
	sprintf (prefs.default_db_file, "C:\\tines_data");
#endif
	
	pid = getpid();

	cli_add_string ("format", prefs.format, "The format of the current file.");
	cli_add_string ("rc_file", prefs.rc_file, "The path and name of the current rc file.");
	cli_add_string ("db_file", prefs.db_file, "The path and name of the current database file.");
	cli_add_string ("default_db_file", prefs.default_db_file, "The path and name of the default database file.");

	cli_add_int ("showpercent", &prefs.showpercent, "If true, a todo entry with children shows the percentage of children done.");
	cli_add_int ("fixedfocus", &prefs.fixedfocus, "If true, anchors the selection bar about 1/3 of the way down the screen.");
	cli_add_int ("savepos", &prefs.savepos, "If true, saves the current position.");
	cli_add_int ("saveexpand", &prefs.saveexpand, "If true, saves the current expansion state.");
	cli_add_int ("readonly", &prefs.readonly, "If true, the database is read-only.");
	cli_add_int ("pid", &pid, "The process ID for this instance of Tines.");

#ifdef NCURSES_VERSION 
	cli_add_int ("escdelay", (long *)&ESCDELAY,
		"How long curses waits before it decides ESC is ESC and not a coded key sequence.");
#endif
}

void write_default_prefs ()
{
	FILE *file;
	char infile[MAXPATHLEN];

	/* set up path to master rc file */
	snprintf(infile, MAXPATHLEN, "%s%s", SHAREDIR, RCFILEIN);

	if (file_check(infile)) {
		cp_file(infile, prefs.rc_file);
	} else {
		/* use minimal rc file */
		file = fopen (prefs.rc_file, "w");
		fprintf (file,
#include "../doc/minimal.inc"
		);
		fclose (file);
	}
}

void load_prefs (void)
{
	if (xml_check (prefs.rc_file)) {
		printf
			("Your current ~/.hnbrc is outdated (it's XML; the new format\n\
is plain text... remove it and let Tines make a new default)\n");
		exit (0);
	}
	cli_load_file (prefs.rc_file);
}
