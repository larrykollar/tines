/*
 * sigs.c -- Tines suspend/resume
 *
 * Copyright (C) 2016 Larry Kollar <lkollar@gmail.com>
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

#include <signal.h>
#include <curses.h>
#include "cli.h"
#include "node.h"
#include "prefs.h"

static void* suspend(int argc, char **argv, void *data)
{
	Node *node = (Node *) data;

	if(prefs.ui == 1) { /* curses mode */
		/* echo();
		noraw(); */
		def_prog_mode();
		endwin();
	}

	raise( SIGTSTP ); /* drop back to shell (need equiv for Doze) */

	/* 'fg' returns here & we can restore stuff. */

	if(prefs.ui == 1) { /* curses mode */
		/* noecho();
		raw();
		refresh(); */
		reset_prog_mode();
		refresh();
	}
	return node;
}

/*
!init_sigs();
*/
void init_sigs (void)
{
	cli_add_command ("stop", suspend, "");
	cli_add_help ("stop", "Stops Tines without quitting. Use fg at the shell prompt to resume.");
}
