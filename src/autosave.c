/*
 * autosave.c -- the core of the autosave functionality
 *             
 *
 * Copyright (C) 2001,2003 �yvind Kol�s <pippin@users.sourceforge.net>
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
#include <unistd.h>
#include "tree.h"
#include "prefs.h"
#include "ui.h"
#include "libcli/cli.h"

static long nodes_changed=0;        /* counter for number of changes since saves */
static long autosave_threshold=0;   /* autosave for every autosave_threshold nodes_changed */
static long autosave_timeout=50;    /* ticks to wait for autosaving, if there is nodes changed */
static long autosave_timer=0;
static long autosave_sync=1;

static void autosave_invoke(Node *pos){

	if (prefs.db_file[0]!= (char) 255) { /* magic value when tutorial is shown */
		{
			char buf[4096];
			
#ifdef USE_NARROW_MODE
			if (global_tree_narrow.is_narrowed) {
				pos = tree_narrow_suspend (pos, &global_tree_narrow);
			}
#endif
			sprintf (buf, "export_hnb %s_tines_rescue", prefs.db_file);		
			docmd (node_root(pos), buf);
			cli_outfunf("autosaved, %i nodes changed\n",nodes_changed);
#ifdef USE_NARROW_MODE
			if (global_tree_narrow.suspend) {
				pos = tree_narrow_unsuspend (pos, &global_tree_narrow);
			}
#endif
		}
	}
	if(autosave_sync)
		sync();
	nodes_changed=0;
	autosave_timer=0;
}

static void* tree_changed_cmd (int argc, char **argv, void *data)
{
		/* TODO: add increment handling, for "extreme changes" */
	Node *pos = (Node *) data;
    
	nodes_changed++;
	if(autosave_threshold && autosave_threshold<=nodes_changed)
		autosave_invoke(pos);
	return pos;
}

static void* autosave_check_timeout (int argc, char **argv, void *data)
{
	Node *pos = (Node *) data;
    
	if(nodes_changed){
		autosave_timer++;
		if(autosave_timeout && autosave_timeout < autosave_timer){
			autosave_invoke(pos);
		}		
	}
	return pos;
}

/*
!init_autosave();
*/
void init_autosave ()
{
	cli_add_command ("tree_changed", tree_changed_cmd, "[increment]");
	cli_add_help ("tree_changed",
				  "Used internally to drive the autosave functionality. For severe changes, pass a high number(1000), to make sure the radical changes are saved immediately. Otherwise, don't pass an increment.");
	cli_add_command ("autosave_check_timeout", autosave_check_timeout, "");
	cli_add_help ("autosave_check_timeout",
			"Checks the timeout counter and autosaves if necessary.");
	cli_add_int ("autosave_timeout", &autosave_timeout, "The number of ticks before autosaving after a change.");
	cli_add_int ("autosave_sync", &autosave_sync, "True if the filesystem should be synced after autosave.");
	cli_add_int ("autosave_timer", &autosave_timer, "The number of ticks since the first unsaved change.");
	cli_add_int ("autosave_threshold", &autosave_threshold,
		"Automatically save after this many nodes are changed.");
	cli_add_int ("autosave_threshold_nodes_changed", &nodes_changed,
		"A counter for the number of changes since the last save.");
}
