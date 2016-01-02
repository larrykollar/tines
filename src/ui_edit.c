/*
 * ui_edit.c -- lineeditor/(readline like stuff) and bindings side of nodecontent editor
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

#include <ctype.h>
#include <string.h>
#include <unistd.h>

#include "tree.h"
#include "ui.h"
#include "prefs.h"
#include "ui_overlay.h"
#include "evilloop.h"
#include <stdlib.h>

static void* ui_edit_cmd (int argc, char **argv, void *data)
{
	Tbinding *c;
	int stop = 0;
	static int cursor_pos;

/*	static char *data_backup;*/

	Node *node_backup;

	int tempscope = ui_current_scope;
	char input[BUFFERLENGTH];
	Node *pos = (Node *) data;

	ui_current_scope = ui_scope_nodeedit;

	memset (input, 0, sizeof (input));
	if(prefs.readonly){
		cli_outfun("readonly flag set, avoiding tree change");
		ui_current_scope = tempscope;
		return data;
	}

	if (inputbuf[0]) {			/* there is data in the inputbuffer,.. 
								   we should not edit, but handle that
								   data instead...  this doesn't really belong here,.. but the hack works
								 */
		if (node_getflag (pos, F_temp)) {
			node_setflag (pos, F_temp, 0);
		} else {
			pos = node_insert_down (node_bottom (pos));
			node_set (pos, TEXT, inputbuf);
			if (!strcmp(fixnullstring(node_get(node_left(pos),"type")),"todo")){
				node_set (pos, "type","todo");
				node_set (pos, "done","no");
			}
		}
		ui_current_scope = tempscope;
		docmd(pos,"tree_changed");
		return pos;
	}

	node_backup = node_duplicate (pos);
	input[0] = 0;
	strcpy (&input[0], fixnullstring (node_get (pos, TEXT)));
	cursor_pos = strlen (input);

	input[cursor_pos] = ' ';
	input[cursor_pos + 1] = 0;
	input[cursor_pos + 2] = 0;

	while (!stop) {
		node_set (pos, TEXT, input);
		ui_draw (pos, (char *) (intptr_t) cursor_pos, 1);
		c = parsekey (ui_input (), ui_scope_nodeedit);
		switch (c->action) {
			case ui_action_right:
				if (cursor_pos < (strlen (input) - 1))
					cursor_pos++;
				break;
			case ui_action_left:
				if (cursor_pos)
					cursor_pos--;
				break;
			case ui_action_skipword:
				if (cursor_pos < (strlen (input) - 1))
					cursor_pos++;
				while ((cursor_pos < (strlen (input) - 1))
					   && isalpha (input[cursor_pos]))
					cursor_pos++;
				while ((cursor_pos < (strlen (input) - 1))
					   && !isalpha (input[cursor_pos]))
					cursor_pos++;
				break;
			case ui_action_bskipword:
				if (cursor_pos)
					cursor_pos--;
				while (cursor_pos && !isalpha (input[cursor_pos]))
					cursor_pos--;
				while ((cursor_pos - 1) && isalpha (input[cursor_pos - 1]))
					cursor_pos--;
				if (isalpha (input[cursor_pos - 1]))
					cursor_pos--;
				break;
			case ui_action_kill_line:
				cursor_pos = 0;
				input[cursor_pos] = ' ';
				input[cursor_pos + 1] = 0;
				input[cursor_pos + 2] = 0;
				break;
			case ui_action_bol:
				cursor_pos = 0;
				break;
			case ui_action_eol:
				cursor_pos = strlen (input) - 1;
				break;
			case ui_action_up:
				if (hnb_edit_posup >= 0)
					cursor_pos = hnb_edit_posup;
				break;
			case ui_action_down:
				if (hnb_edit_posdown < strlen (input))
					cursor_pos = hnb_edit_posdown;
				else
					cursor_pos = strlen (input) - 1;
				break;
			case ui_action_cancel:
				node_set (pos, TEXT,
						  fixnullstring (node_get (node_backup, TEXT)));
				stop = 1;
				break;
			case ui_action_confirm:
				input[strlen (input) - 1] = 0;
				node_set (pos, TEXT, input);
				if (node_getflag (pos, F_temp))
					node_setflag (pos, F_temp, 0);
				stop = 1;
				docmd(pos, "tree_changed");  
				break;
			case ui_action_delete:
				if (cursor_pos < (strlen (input) - 1)) {
					cursor_pos++;
					if (cursor_pos) {
						memmove (&input[cursor_pos - 1],
								 &input[cursor_pos],
								 strlen (input) - cursor_pos);
						input[strlen (input) - 1] = 0;
						cursor_pos--;
					}
				}
				break;
			case ui_action_backspace:
				if (cursor_pos) {
					memmove (&input[cursor_pos - 1],
							 &input[cursor_pos], strlen (input) - cursor_pos);
					input[strlen (input) - 1] = 0;
					cursor_pos--;
				}
				break;
			case ui_action_split:
				input[strlen (input) - 1] = 0;
				node_insert_down (pos);
				if (input[cursor_pos] == ' ')
					node_set (node_down (pos), TEXT, &input[cursor_pos + 1]);
				else
					node_set (node_down (pos), TEXT, &input[cursor_pos]);
				input[cursor_pos] = ' ';
				input[cursor_pos + 1] = 0;
				break;
			case ui_action_join:
				if (node_down (pos)) {
					cursor_pos = strlen (input);
					strcpy (&input[cursor_pos - 1],
							fixnullstring (node_get (node_down (pos), TEXT)));
					input[strlen (input)] = ' ';
					input[strlen (input) + 1] = 0;
					if (node_right (node_down (pos))) {
						docmd (pos,
							   "status refusing to remove node after join, because it has children");
					} else {
						node_remove (node_down (pos));
					}
					cursor_pos--;
				}
				break;
			case ui_action_unbound:
				undefined_key (ui_scope_names[ui_scope_nodeedit],
							   c->key !=
							   1000 ? c->key : *((int *) &c->
												 action_param[0]));
			case ui_action_ignore:
				break;
			default:
				if (c->action > 31 && c->action < 255) {	/*  input for buffer */
					memmove (&input[cursor_pos + 1],
							 &input[cursor_pos],
							 strlen (input) - cursor_pos + 1);
					input[cursor_pos++] = c->action;
				} else {
					undefined_key (ui_scope_names[ui_scope_nodeedit],
								   c->key !=
								   1000 ? c->key : *((int *) &c->
													 action_param[0]));
				}
				break;
		}
	}
	node_free (node_backup);
	ui_current_scope = tempscope;
	return data;
}

int ui_getstr_loc (char *input, int x, int y, int maxlen)
{
	int stop = 0;
	Tbinding *c;
	long cursor_pos = strlen (input);
	char *data_backup = strdup (input);

	input[cursor_pos] = ' ';
	input[cursor_pos + 1] = 0;
	input[cursor_pos + 2] = 0;


	if (x == -1 && y == -1) {
		getyx (stdscr, y, x);
	}

	while (!stop) {
		move (y, x);
		clrtoeol ();
		move (y, x);

		ui_style (ui_style_menutext);

		addstr (input);
		move (y, x + cursor_pos);

		move (y, x + cursor_pos);

		ui_style (ui_style_selected);
		addch (input[cursor_pos]);
		move (LINES - 1, COLS - 1);

		c = parsekey (ui_input (), ui_scope_lineedit);
		switch (c->action) {
			case ui_action_right:
				if (cursor_pos < (strlen (input) - 1))
					cursor_pos++;
				break;
			case ui_action_left:
				if (cursor_pos)
					cursor_pos--;
				break;
			case ui_action_up:
/*				strcpy(input,cli_historyprev());
				cursor_pos=strlen(input);
				input[cursor_pos] = ' ';
				input[cursor_pos + 1] = 0;
				input[cursor_pos + 2] = 0;
*/ break;
			case ui_action_down:
/*				strcpy(input,cli_historynext());
				cursor_pos=strlen(input);
				input[cursor_pos] = ' ';
				input[cursor_pos + 1] = 0;
				input[cursor_pos + 2] = 0;
*/ break;
			case ui_action_bol:
				cursor_pos = 0;
				break;
			case ui_action_eol:
				cursor_pos = strlen (input) - 1;
				break;
			case ui_action_complete:
				if (cursor_pos == strlen (input) - 1) {
					char *tmpcommand;

					input[cursor_pos] = 0;
					tmpcommand = cli_complete (input);
					strcpy (input, tmpcommand);
					cursor_pos = strlen (tmpcommand);
					input[cursor_pos] = ' ';
					input[cursor_pos + 1] = 0;
					input[cursor_pos + 2] = 0;
					status_draw ();
					refresh ();
				}
				break;
			case ui_action_cancel:
				strcpy (&input[0], data_backup);
				input[strlen (input) - 1] = 0;
				stop = 1;
				input[0] = 0;
				break;
			case ui_action_confirm:
				input[strlen (input) - 1] = 0;
/*				cli_historyadd(input);
*/ stop = 1;
				break;
			case ui_action_delete:
				if (cursor_pos < (strlen (input) - 1)) {
					cursor_pos++;
					if (cursor_pos) {
						memmove (&input[cursor_pos - 1],
								 &input[cursor_pos],
								 strlen (input) - cursor_pos);
						input[strlen (input) - 1] = 0;
						cursor_pos--;
					}
				}
				break;
			case ui_action_backspace:
				if (cursor_pos) {
					memmove (&input[cursor_pos - 1],
							 &input[cursor_pos], strlen (input) - cursor_pos);
					input[strlen (input) - 1] = 0;
					cursor_pos--;
				}
				break;

			default:
				if (c->action > 31 && c->action < 255) {	/*  input for buffer */
					memmove (&input[cursor_pos + 1],
							 &input[cursor_pos],
							 strlen (input) - cursor_pos + 1);
					input[cursor_pos++] = c->action;
				}
				break;
		}
	}


	free (data_backup);

	return (input[0]);
}


void ui_getstr (char *prompt, char *datastorage)
{
	move (LINES - 2, 0);
	ui_style (ui_style_menuitem);
	addstr (prompt);
	clrtoeol ();

	move (LINES - 1, 0);
	ui_style (ui_style_menuitem);
	addstr (">");
	ui_style (ui_style_menutext);
	addch (' ');

	ui_getstr_loc (datastorage, -1, -1, 80);
}

/*
!init_ui_edit();
*/

void init_ui_edit ()
{
	cli_add_command ("edit", ui_edit_cmd, "");
	cli_add_help ("edit", "Invokes the node editor for the current node.");
}
