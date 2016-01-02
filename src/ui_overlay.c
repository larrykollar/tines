/*
 * ui_overlay.c -- help and status display for hnb
 *
 * Copyright (C) 2001-2003 Øyvind Kolås <pippin@users.sourceforge.net>
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "ui.h"
#include "ui_binding.h"
#include "evilloop.h"

/*	ui_overlay
	
	current node
	dirtyness
	
	scope_no (and thus name and help texts)
	
	status
	status_display_counter,..
*/


static char *ui_helptext[MAX_SCOPES] = { 0 };

static void* ui_helptext_cmd (int argc, char **argv, void *data)
{
	if(argc>1)
		ui_helptext[ui_current_scope] = strdup (argv[1]);
	return data;
}

#define MAX_STATUS_LINES 100

static char status_line[MAX_STATUS_LINES][128] = { "" };

static int status_ttl = 0;

static void status (char *message, int ttl)
{
	int i;

	for (i = 0; i < MAX_STATUS_LINES - 1; i++)
		strncpy (status_line[i], status_line[i + 1], 128);
	strncpy (&status_line[MAX_STATUS_LINES - 1][0], message, 128);
	status_ttl += ttl;
	if (status_ttl >= MAX_STATUS_LINES)
		status_ttl = MAX_STATUS_LINES - 1;
	if (status_ttl >= LINES - 2)
		status_ttl = LINES - 3;


}

void set_status (char *message)
{
	char *tbuf, *word, *bp, *dp, *wp;
	int width;

	if (!COLS)
		width = 60;
	else
		width = COLS + 1 - 2;

	bp = tbuf = malloc (width);
	wp = word = malloc (width);
	dp = message;

	*bp = *wp = '\0';

	while (1 + 1 == 2) {
		if (isspace ((unsigned char)*dp) || *dp == '\0') {
			if ((bp - tbuf) + (wp - word) + 1 < width) {
				strcpy (bp, word);
				bp += (wp - word);
				*(bp++) = ' ';
				*bp = '\0';
				wp = word;
				*wp = '\0';
			} else {
				status (tbuf, 1);
				bp = tbuf;
				*bp = '\0';
				strcpy (bp, word);
				bp += (wp - word);
				*(bp++) = ' ';
				*bp = '\0';
				wp = word;
				*wp = '\0';
			}
			if (!*dp)
				break;
		} else {
			if (wp - word >= width - 1) {
				status (tbuf, 1);
				status (word, 1);
				wp = word;
			}
			*(wp++) = *dp;
			*wp = '\0';
		}
		dp++;
	}
	status (tbuf, 1);

	free (word);
	free (tbuf);
}


static void* ui_status_cmd (int argc, char **argv, void *data)
{
	if(argc==2 && (!strcmp(argv[1],"-c") || !strcmp(argv[1],"--clear"))){
		status_ttl=0;
	} else if(argc>1){ /* FIXME: should handle more than one string on commandline */
		set_status (argv[1]);
		if(!ui_inited)
		cli_outfun(argv[1]);
	}
	return data;
}


void status_draw (void)
{
	int j;

	for (j = 0; j < status_ttl; j++) {
		move (status_ttl - j - 1, 0);
		ui_style (ui_style_menuitem);
		addstr (" ");
		ui_style (ui_style_background);
		addstr (" ");
		ui_style (ui_style_menutext);
		addstr (status_line[MAX_STATUS_LINES - j - 1]);
		move (status_ttl - j - 1,
			  strlen (status_line[MAX_STATUS_LINES - j - 1]) + 2);
		clrtoeol ();
	}
	if (status_ttl > 0)
		status_ttl--;
}

void help_draw (int scope)
{
	if (!ui_inited)
		return;
	status_draw ();

	move (LINES - 1, 0);
	ui_style (ui_style_menuitem);
	{
		unsigned char *p = (unsigned char *) ui_helptext[scope];
		int style_is_menuitem = 1;

		while (*p) {
			switch (*p) {
				case '|':
					if (*(p + 1) == '|') {
						addch ('|');
						p++;
					} else {
						if (style_is_menuitem) {
							ui_style (ui_style_menutext);
						} else {
							ui_style (ui_style_menuitem);
						}
						style_is_menuitem = !style_is_menuitem;
					}
					break;
				default:
					addch (*p);
					break;
			}
			p++;
		}
	}


	clrtoeol ();
	ui_style (ui_style_background);

}

/*
!init_ui_overlay();
*/
void init_ui_overlay ()
{
	cli_add_command ("helptext", ui_helptext_cmd, "<help for context>");
	cli_add_help ("helptext",
				  "Defines the helptext for the current context, the character | alternates between the menuitem and the menutext styles, || is the escape sequence for a single pipe.");
	cli_add_command ("status", ui_status_cmd, "<-c|--clear|message>");
	cli_add_command ("echo", ui_status_cmd, "<-c|--clear|message>");
	cli_add_help ("status", "Adds 'message' as the newest status line, if -c or --clear\
 is specified, all pending status messages will be cleared off the screen");
	cli_add_help("echo","alias for status");

}
