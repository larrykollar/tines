/*
 * file_help.c -- tutorial import and export filters for hnb
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "tree.h"
#include "file.h"
#include "query.h"
#include "util_string.h"

/* *INDENT-OFF* */

static char *helpquote[]={
	"\"", "\\\\",
	"\"", "\\\"",NULL
};

/* *INDENT-ON* */

static void* export_help (int argc, char **argv, void *data)
{
	Node *node = (Node *) data;
	char *filename = argc==2?argv[1]:"";
	Node *tnode;
	int level, flags, startlevel, lastlevel, cnt;
	char *cdata;
	FILE *file;

	file = fopen (filename, "w");
	if (!file) {
		cli_outfunf ("help export, unable to open \"%s\"", filename);

		return node;
	}
	startlevel = nodes_left (node);

	tnode = node;
	lastlevel = 0;
	fprintf (file,
			 "#define i(a,b,c) do{Node *tnode=node_new();node_set(tnode,TEXT,b);node_setflags(tnode,c);import_node(&ist,a,tnode);}while(0)\n\n");
	while ((tnode != 0) & (nodes_left (tnode) >= startlevel)) {
		char *quoted;
		level = nodes_left (tnode) - startlevel;
		flags = node_getflags (tnode);
		cdata = fixnullstring (node_get (tnode, TEXT));

		quoted=string_replace(cdata,helpquote);

		for (cnt = 0; cnt < level; cnt++)
			fprintf (file, "\t");

		fprintf (file, "i(%i,\"%s\",%i);\n", level, quoted,
				 flags);

		free(quoted);
		lastlevel = level;
		tnode = node_recurse (tnode);
	}
	level = 0;

	fclose (file);

	cli_outfunf ("help export, wrote data to \"%s\"", filename);
	return node;
}

static void* import_help (int argc, char **argv, void *data)
{
	Node *node = (Node *) data;
	import_state_t ist;

	init_import (&ist, node);
#include "../doc/tutorial.inc"

	if (node_getflag (node, F_temp))
		node = node_remove (node);

	return node;
}

/*
!init_file_help();
*/
void init_file_help ()
{
	cli_add_command ("export_help", export_help, "<filename>");
	cli_add_command ("import_help", import_help, "<filename>");
}
