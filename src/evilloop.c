/*
 * evilloop.c -- The event loop / heart of execution for hnb
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


#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "tree.h"
#include "tree_todo.h"
#include "ui.h"
#include "file.h"

#include "prefs.h"

#include "ui_cli.h"
#include "cli.h"
#include "evilloop.h"
#include "util_string.h"

char inputbuf[BUFFERLENGTH];

static long forced_up = 0;
static long forced_down = 0;
static long add_at_top = 0;

int hnb_nodes_down;
int hnb_nodes_up;

const char *collapse_names[] = {
	"all (standard)",
	"all but first level of children",
	"show whole tree",
	"show path of current level",
	""
};

static Node *node_forced_up ( Node *node)
{
	if (node_up (node) && node_getflag( node_up (node), F_expanded)) {
		node = node_up (node);
		while (node_right (node) && node_getflag(node,F_expanded)) {
			node = node_right (node);
			node = node_bottom (node);
		}
		return (node);
	} else {
		if (node_up (node))
			return (node_up (node));
		else
			return (node_left (node));
	}
	return node_left (node);
}

static Node *node_forced_down ( Node *node)
{
	if (node_getflag(node,F_expanded)) {
		return node_recurse (node);
	} else {
		if (node_down (node)) {
			return (node_down (node));
		} else {
			while (node != 0) {
				node = node_left (node);
				if (node_down (node))
					return (node_down (node));
			}
		}
	}
	return NULL;
}


/*  removes *pos if it is a temporary node, then returns 1
 *  otherwize returns 0
 */
static int remove_temp (Node **pos)
{
	if (node_getflag (*pos, F_temp)) {
		*pos = node_remove ((*pos));
		node_update_parents_todo ((*pos));
		docmd(*pos,"tree_changed");
		return 1;
	}
	return 0;
}

static char *no_remove_temp_commands[]={
	"insert_below",
	"edit",
	"indent",
	"outdent",
	"expand",
	"collapse",
	"paste",
	NULL
};

static char *keep_inputbuf[]={
	"indent",
	"outdent",
	"expand",
	"collapse",
	NULL
};

int quit_hnb=0;

static void* cmd_quit(int argc,char **argv,void *data){
	Node *pos=(Node *)data;
	quit_hnb=1;


	return pos;
}

/*
!init_quit();
*/
void init_quit(){
	cli_add_command("quit",cmd_quit,"");
	cli_add_help("quit","quits hnb, no questions asked");
	cli_add_command("q",cmd_quit,"");
	cli_add_help("q","quits hnb, no questions asked");
}

Node *evilloop (Node *pos)
{
	cli_outfun = set_status;

	while (!quit_hnb) {
		Tbinding *binding;

		ui_draw (pos, inputbuf, 0);
		binding = parsekey (ui_input (), ui_current_scope);
		do {

			switch (binding->action) {
				case ui_action_quit:
					remove_temp (&pos);
					quit_hnb = 1;
					break;
				case ui_action_command:
					if(!string_isoneof(binding->action_param, no_remove_temp_commands))
						remove_temp (&pos);
					pos = docmd (pos, binding->action_param);
					if(!string_isoneof(binding->action_param,keep_inputbuf))
						inputbuf[0] = 0;
					break;
				case ui_action_top:
					remove_temp (&pos);
					inputbuf[0] = 0;
					pos = node_top (pos);
					break;
				case ui_action_bottom:
					remove_temp (&pos);
					inputbuf[0] = 0;
					pos = node_bottom (pos);
					break;
				case ui_action_up:
					if (!remove_temp (&pos)) {
						if(forced_up){
							if (node_forced_up (pos)){
								pos = node_forced_up (pos);
							}
						} else {
							if (node_up (pos)){
								pos = node_up (pos);
							}
						}
					}
					inputbuf[0] = 0;
					break;
				case ui_action_down:
					if (!remove_temp (&pos)) {
						if(forced_down){
							if(node_forced_down(pos))
								pos = node_forced_down (pos);
						} else {
							if(node_down(pos))
								pos = node_down (pos);
						}							
						inputbuf[0] = 0;
						break;
					}
				case ui_action_pagedown:
					remove_temp (&pos);
					inputbuf[0] = 0;
					{
						int n;

						for (n = 0; n < hnb_nodes_down; n++)
							if (node_down (pos)) {
								pos = node_down (pos);
							}
					}
					break;
				case ui_action_pageup:
					remove_temp (&pos);
					inputbuf[0] = 0;
					{
						int n;

						for (n = 0; n < hnb_nodes_up; n++)
							if (node_up (pos))
								pos = node_up (pos);
					}
					break;
				case ui_action_left:
					if (!remove_temp (&pos)) {
						if (node_left (pos))
							pos = node_left (pos);
					}
					inputbuf[0] = 0;
					break;
				case ui_action_right:
					if (node_right (pos)) {
						pos = node_right (pos);
					} else {
						if (fixnullstring (node_get (pos, TEXT))[0]) {
							node_insert_right (pos);
							if (node_getflag (pos, F_temp))
								node_setflag (pos, F_temp, 0);
							if (!strcmp(fixnullstring(node_get(pos,"type")),"todo")){
								node_set (node_right (pos), "type","todo");
								node_set (node_right (pos), "done","no");
							}
							node_setflag (node_right (pos), F_temp, 1);
							pos = node_right (pos);
						}
					}
					inputbuf[0] = 0;
					break;
				case ui_action_complete:
					if (strcmp
						(inputbuf,
						 fixnullstring (node_get (pos, TEXT))) == 0) {
						if (node_right (pos)) {
							pos = node_right (pos);
						} else {
							if (fixnullstring (node_get (pos, TEXT))[0]) {
								node_insert_right (pos);
								if (node_getflag (pos, F_temp))
									node_setflag (pos, F_temp, 0);
								if (!strcmp(fixnullstring(node_get(pos,"type")),"todo")){
									node_set (node_right (pos), "type","todo");
									node_set (node_right (pos), "done","no");
								}
								node_setflag (node_right (pos), F_temp, 1);

								pos = node_right (pos);
							}
						}
						inputbuf[0] = 0;
					} else {
						strcpy (inputbuf,
								fixnullstring (node_get (pos, TEXT)));
					}
					break;
				case ui_action_cancel:
					if (node_getflag (pos, F_temp)) {
						pos = node_remove (pos);
					} else {
						/*stop = ui_quit (pos); */
					}
					inputbuf[0] = 0;
					break;
				case ui_action_backspace:
					if (!strlen (inputbuf)) {
						/*pos = ui_remove (pos); */
					} else {
						inputbuf[strlen (inputbuf) - 1] = 0;
						if (node_getflag (pos, F_temp))
							if (node_up (pos))
								pos = node_remove (pos);
					}
					break;
				case ui_action_unbound:
					undefined_key (ui_scope_names[ui_current_scope],
								   binding->key !=
								   1000 ? binding->key : *((int *) &binding->
														   action_param[0]));
				case ui_action_ignore:
					break;
				default:
					if (binding->action > 31 && binding->action < 255) {	/*  input for buffer */
						inputbuf[strlen (inputbuf) + 1] = 0;
						inputbuf[strlen (inputbuf)] = binding->action;
					} else
						undefined_key (ui_scope_names[ui_current_scope],
									   binding->key !=
									   1000 ? binding->
									   key : *((int *) &binding->
											   action_param[0]));
					break;
			}
		} while ((++binding)->key == 999);


		if (strlen (inputbuf)) {
			if (node_getflag (pos, F_temp)) {
				node_set (pos, TEXT, inputbuf);
			} else {
				if (node_match (inputbuf, pos)) {
					pos = node_match (inputbuf, pos);
				} else {
				  if (add_at_top) {
					pos = node_insert_up (node_top (pos));
				  } else {
					pos = node_insert_down (node_bottom (pos));
				  }
					node_setflag (pos, F_temp, 1);
					node_set (pos, TEXT, inputbuf);
					if (node_left (pos))
							if (!strcmp(fixnullstring(node_get(node_left(pos),"type")),"todo")){
								node_set (pos, "type","todo");
								node_set (pos, "done","no");
							}
				}
			}
		} else {
			docmd(pos, "autosave_check_timeout"); 
		}
	}
	return pos;
}

/*
!init_evilloop();
*/

void init_evilloop ()
{
	cli_add_int ("forced_up", &forced_up,
				 "whether movement upwards is forced beyond first sibling");
	cli_add_int ("forced_down", &forced_down,
				 "whether movement downwards is forced beyond last sibling");
	cli_add_int ("add_at_top", &add_at_top,
				 "whether items inserted automatically are added at the top (default 0, at the bottom)");
}
