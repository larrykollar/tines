/*
 * state.c -- a simplistic undo/redo kind of system
 *
 * Copyright (C) 2003 Øyvind Kolås <pippin@users.sourceforge.net>
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tree.h"
#include "file.h"
#include "prefs.h"
#include "ui_cli.h"

static Node *savedtree = NULL;

static void* save_state_cmd (int argc, char **argv, void *data)
{
	Node *pos = (Node *) data;
	Node *i;
	Node *j;

	if (savedtree != NULL) {
		tree_free (savedtree);
	}
	savedtree = node_new ();

	i = node_root (pos);
	j = savedtree;
	do {
		j = savedtree = tree_duplicate (i, j);
		i = node_down (i);
		j = node_insert_down (j);
	} while (i != NULL);
	j = node_remove (j);

	{
		int no;

		no = node_no (pos);
		savedtree = node_root (savedtree);
		while (--no)
			savedtree = node_recurse (savedtree);
	}

	return pos;
}

static void* restore_state_cmd (int argc, char **argv, void *data)
{
	Node *pos = (Node *) data;

	if (savedtree != NULL) {
		Node *temp;

		temp = pos;
		pos = savedtree;
		savedtree = temp;
		tree_free (savedtree);
		savedtree = NULL;
	}
	return pos;
}

/*
!init_keepstate();
*/
void init_keepstate ()
{
	cli_add_command ("save_state", save_state_cmd, "");
	cli_add_help ("save_state",
				  "Saves a copy of the current tree and selected node in memory");
	cli_add_command ("restore_state", restore_state_cmd, "");
	cli_add_help ("restore_state",
				  "Restores the copy of the tree saved with save_state");
}
