/*
 * file.c -- utility functions for import/export tines
 *
 * Copyright (C) 2001-2003 Øyvind Kolås <pippin@users.sourceforge.net>
 * Modified for Tines by Larry Kollar, 2016
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/param.h>

#include "tree.h"
#include "file.h"
#include "prefs.h"
#include "cli.h"
#include "ui_cli.h"

void init_import (import_state_t * is, Node *node)
{
	is->npos = node;
	is->startlevel = nodes_left (node);
}

/*
 *
 * @return the node inserted
 * */
Node *import_node_text (import_state_t * is, int level, char *data)
{
	int node_level;

	level += is->startlevel;

	while ((node_level = nodes_left (is->npos)) > level)
		is->npos = node_left (is->npos);
	if (node_level == level)
		is->npos = node_insert_down (is->npos);
	if (node_level < level)
		is->npos = node_insert_right (is->npos);
	node_set (is->npos, TEXT, data);
	return is->npos;
}



/*
 *
 * @return the node inserted, no need to free the node afterwards
 * */
Node *import_node (import_state_t * is, int level, Node *node)
{
	int node_level;

	level += is->startlevel;

	while ((node_level = nodes_left (is->npos)) > level)
		is->npos = node_left (is->npos);
	if (node_level == level)
		is->npos = node_insert_down (is->npos);
	if (node_level < level)
		is->npos = node_insert_right (is->npos);
	node_swap (node, is->npos);
	node_free (is->npos);
	is->npos = node;
	return is->npos;
}



/* returns 1 if the first couple of lines of file contains 'xml' */
int xml_check (char *filename)
{
	FILE *file;
	char buf[bufsize];
	int j;

	file = fopen (filename, "r");
	if (file == NULL)
		return -1;

	for (j = 0; j < 2; j++) {
		if (fgets (buf, bufsize, file) == NULL) {
			fclose (file);
			return 0;
		}
		if (strstr (buf, "xml") != 0) {
			fclose (file);
			return 1;
		}
	}
	fclose (file);
	return 0;
}

/* returns the node number stored in the comment, if available  */
int xml_getpos (char *filename)
{
	FILE *file;
	char buf[bufsize];
	char *s;
	int j;

	file = fopen (filename, "r");
	if (file == NULL)
		return -1;

	for (j = 0; j < 2; j++) {
		if (fgets (buf, bufsize, file) == NULL) {
			fclose (file);
			return 0;
		}
		if ((s = strstr (buf, "<?tines pos=\""))) {
			fclose (file);

			return (int)strtol ( s+strlen("<?tines pos=\""), NULL, 10 );
		}
        /* handle legacy processing instructions */
		if ((s = strstr (buf, "<?hnb pos=\""))) {
			fclose (file);

			return (int)strtol ( s+strlen("<?hnb pos=\""), NULL, 10 );
		}
		if ((s = strstr (buf, "<?pos=\""))) {
			fclose (file);

			return (int)strtol ( s+strlen("<?pos=\""), NULL, 10 );
		}
	}
	fclose (file);
	return -1;
}


/*returns 1 if file exists*/
int file_check (char *filename)
{
	FILE *file;

	file = fopen (filename, "r");
	if (file == NULL)
		return 0;
	fclose (file);
	return 1;
}

/* expands leading ~/ to user's home dir, returns the expanded path/file */
char* fn_expand( char* s )
{
	static char expanded[MAXPATHLEN];

	if( !strncmp(s, "~/", 2) ) {
        strcpy( expanded, getenv("HOME") );
        if( strlen(expanded) + strlen(s)-1 >= MAXPATHLEN ) {
            cli_outfunf ("Path too long, using /tmp/tines.toolong");
            return( "/tmp/tines.toolong" );
        } else {
            strcat( expanded, s+1 );
            return( expanded );
        }
    } else {
        return( s );
    }
}


static void* cmd_save (int argc, char **argv, void *data)
{
	Node *pos = (Node *) data;

	if(prefs.readonly){
		docmd (pos, "status \"Read-only mode, not writing to disk.\"\n");
		return pos;
	}

#ifdef USE_NARROW_MODE
	if (global_tree_narrow.is_narrowed) {
		pos = tree_narrow_suspend (pos, &global_tree_narrow);
	}
#endif /*USE_NARROW_MODE*/
	
	if (prefs.db_file[0] != (char) 255) { /* magic value of tutorial */
		{
			char buf[4096];
			char swapfile[MAXPATHLEN];

			sprintf(swapfile,"%s_tines_rescue",prefs.db_file);
			/* remove(swapfile); when not removing it works as a lockfile */
			docmd (pos, "autosave_threshold_nodes_changed 0");

			if (!strcmp(prefs.format,"hnb") || !strcmp(prefs.format,"opml") ||
				!strcmp(prefs.format,"xml")) {
				sprintf (buf, "export_%s \'%s\' %i", prefs.format, prefs.db_file,
						 node_no (pos) - 1);
			} else {
				sprintf (buf, "export_%s \'%s\'", prefs.format, prefs.db_file);
			}
			docmd (node_root (pos), buf);
		}
	} else {
		/* make tutorial users initial database, if initial database dont exist */
	}

#ifdef USE_NARROW_MODE
	if (global_tree_narrow.suspend) {
		pos = tree_narrow_unsuspend (pos, &global_tree_narrow);
	}
#endif /*USE_NARROW_MODE*/

	return pos;
}

static void* cmd_revert (int argc,char **argv, void *data)
{
	Node *pos = (Node *) data;

	if (prefs.db_file[0] != (char) 255) {
		{
			char buf[4096];

			sprintf (buf, "import_%s %s", prefs.format, prefs.db_file);
			node_free(pos);
			pos=tree_new();
			
			pos=docmd (pos, buf);
		}
	}
	return pos;
}


/*
!init_file();
*/
void init_file ()
{
	cli_add_command ("save", cmd_save, "");
	cli_add_help ("save", "Saves the open tree.");

	cli_add_command ("revert", cmd_revert, "");
	cli_add_help ("revert", "Reverts to the last saved version.");
}
