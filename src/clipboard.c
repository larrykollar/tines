/*
 * clipboard.c -- cut,copy and paste for hnb
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


#include "tree.h"
#include "prefs.h"
#include "ui.h"
#include "libcli/cli.h"
/************************* clipboard ************************************/


static Node *clipboard = NULL;

static void* copy_cmd (int argc,char **argv, void *data)
{
	Node *pos = (Node *) data;

	if (clipboard != NULL) {
		tree_free (clipboard);
	}
	clipboard = node_new ();

	clipboard = tree_duplicate (pos, clipboard);
	return pos;
}

static void* cut_cmd (int argc,char **argv, void *data)
{
	Node *pos = (Node *) data;
	if(prefs.readonly){
		cli_outfun("readonly flag set, avoiding tree change");
		return data;
	}

	if (clipboard != NULL) {
		tree_free (clipboard);
	}
	clipboard = node_new ();

	clipboard = tree_duplicate (pos, clipboard);
	pos = node_remove (pos);
	docmd(pos,"tree_changed");
	return pos;
}

static void* paste_cmd (int argc,char **argv, void *data)
{
	Node *pos = (Node *) data;

	if (clipboard == NULL) {
		docmd (pos, "status no data in clipboard");
	} else {
		Node *temp;
		if(prefs.readonly){
			cli_outfun("readonly flag set, avoiding insertion");
			return data;
		}

		temp = node_insert_down (pos);
		tree_duplicate (clipboard, temp);
		docmd(pos,"tree_changed");
	}
	return pos;
}

/*
!init_clipboard();
*/
void init_clipboard ()
{
	cli_add_command ("copy", copy_cmd, "");
	cli_add_help ("copy",
				  "Stores the current node, and it's subtree in the clipboard");
	cli_add_command ("cut", cut_cmd, "");
	cli_add_help ("cut",
				  "Moves the current node, and it's subtree to the clipboard");
	cli_add_command ("paste", paste_cmd, "");
	cli_add_help ("paste",
				  "Inserts the contents of the clipboard at the current position in the tree.");
}
