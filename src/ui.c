/*
 * ui.c -- the part of hnb that calls curses
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

#include "config.h"

#include <curses.h>
#include "tree.h"
#include "ui_overlay.h"
#include "ui.h"
#include "prefs.h"
#include "cli.h"

#define UI_C
#include "ui.h"

long nodes_above;
long active_line;
long nodes_below;
int ui_inited = 0;

void ui_init ()
{
	initscr ();
	clear ();
	intrflush (stdscr, TRUE);
	keypad (stdscr, TRUE);
	nonl ();
	raw ();						/* enable binding ctl+c,ctrl+q,ctrl+z .. */
	noecho ();

	if (prefs.fixedfocus) {
		active_line = LINES / 3;
	} else {
		active_line = 1;
	}

#ifdef NCURSES_VERSION
	/* 20021129 RVE - assign terminal default colors to color number -1 */
	use_default_colors ();

	define_key ("\x7f", KEY_BACKSPACE);
	define_key ("\033[11~", KEY_F (1));

	define_key ("\033[28-", KEY_F (1));
	define_key ("\033OP", KEY_F (1));
	define_key ("\033[OP", KEY_F (1));

	define_key ("\033[12~", KEY_F (2));
	define_key ("\033[13~", KEY_F (3));

	define_key ("\033[A", KEY_UP);
	define_key ("\033OA", KEY_UP);
	define_key ("\033[B", KEY_DOWN);
	define_key ("\033OB", KEY_DOWN);
	define_key ("\033[C", KEY_RIGHT);
	define_key ("\033OC", KEY_RIGHT);
	define_key ("\033[D", KEY_LEFT);
	define_key ("\033OD", KEY_LEFT);


	define_key ("\033[3~", KEY_DC);
	define_key ("\033[7~", KEY_HOME);
	define_key ("\033[8~", KEY_END);

	define_key ("\033[OH", KEY_HOME);
	define_key ("\033[OF", KEY_END);


	define_key ("\033O2A", KEY_SPREVIOUS);
	define_key ("\033[a", KEY_SPREVIOUS);
	define_key ("\033O2B", KEY_SNEXT);
	define_key ("\033[b", KEY_SNEXT);
	define_key ("\033[c", KEY_SRIGHT);
	define_key ("\033[d", KEY_SLEFT);
	
	
	define_key ("\033O2C", KEY_SRIGHT);
	define_key ("\033O2D", KEY_SLEFT);
	
	
#endif

	ui_inited = 1;
	cli_width = COLS;

	if (has_colors ()) {
		start_color ();
	}

	ui_style_restore_color ();
	nodes_above = active_line;
	nodes_below = LINES - active_line;
}


void ui_end ()
{
	clear ();
	refresh ();
	endwin ();
	ui_inited = 0;
}


/*extern Node *pos;
*/
ui_keycode ui_input ()
{
	int c;
	refresh();
	c = getch ();

	ui_keycode result;
	result.key = c;
	result.is_meta = 0;

	if (c == 27) {		// Escape or Meta?
		nodelay(stdscr, 1);
		int nextk = getch();
		if (nextk != ERR) {
			result.key = nextk;
			result.is_meta = 1;
		}
		nodelay(stdscr, 0);
	}

	switch (c) {
#ifdef KEY_RESIZE
		case KEY_RESIZE:
			if (prefs.fixedfocus) {
				active_line = LINES / 3;
			}
			cli_width = COLS;
			c = getch ();
			result.key = ui_action_ignore;
			return result;
#endif
	}

	//return (c);
	return result;
}

void do_undefined_key (char * scope, Tbinding * binding) {
	/* if it's any-key, the key itself is stored in a different place...  (see ui_binding.c)
	   this logic used to be copy-pasted around in several places; I just transplanted it */
	if (binding->key != ui_action_ignore) {
		if (binding->key == 1000) {
			binding->key = *((int *)&binding->action_param[0]);
			docmdf(NULL,"status \"No action assigned to '%s'(%id) in %s-mode\"",tidy_keyname(binding),binding->key,scope);
			binding->key = 1000;
		} else {
			docmdf(NULL,"status \"No action assigned to '%s'(%id) in %s-mode\"",tidy_keyname(binding),binding->key,scope);
		}
	}
}

