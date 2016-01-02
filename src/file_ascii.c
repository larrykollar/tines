/*
 * file_ascii.c -- ascii import and export filters for hnb
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

#include <string.h>
#include <stdio.h>

#include "cli.h"
#include "tree.h"
#include "file.h"
#include "query.h"

#define indent(count,char)	{int j;for(j=0;j<count;j++)fprintf(file,char);}

static long ascii_margin = -1;

static void* import_ascii (int argc, char **argv, void *data)
{
	Node *node = (Node *) data;
	char *filename = argc==2?argv[1]:"";
	int level;
	import_state_t ist;
	char cdata[bufsize];
	FILE *file;

	file = fopen (filename, "r");
	if (file == NULL) {
		cli_outfunf ("ascii import, unable to open \"%s\"", filename);
		return node;
	}

	init_import (&ist, node);

	while (fgets (cdata, bufsize, file) != NULL) {
		level = 0;

		/*strip newlines and carrier return  */
		while (cdata[strlen (cdata) - 1] == 13
			   || cdata[strlen (cdata) - 1] == 10)
			cdata[strlen (cdata) - 1] = 0;

		while (cdata[level] == '\t')	/* find the level of this node */
			level++;

		import_node_text (&ist, level, &cdata[level]);
	}

	fclose (file);

	if (node_getflag (node, F_temp))
		node = node_remove (node);	/* remove temporary node, if tree was empty */

	cli_outfunf ("ascii import, imported \"%s\"", filename);


	return node;
}

static void ascii_export_node (FILE * file, int level, int flags, char *data)
{

	indent (level, "\t");
#if 0
	if (flags & F_todo) {		/* print the flags of the current node */
		if (flags & F_done)
			fprintf (file, "[X]");
		else
			fprintf (file, "[ ]");
	}
#endif
	if (ascii_margin < 0) {		/* no wrap */
		fprintf (file, "%s\n", data);
	} else {
		fprintf (file, "%s\n", data);
	}
}

static void *export_ascii (int argc, char **argv, void *data)
{
	Node *node = (Node *) data;
	char *filename = argc==2?argv[1]:"";
	Node *tnode;
	int level, flags, startlevel;
	char *cdata;
	FILE *file;

	if (!strcmp (filename, "-"))
		file = stdout;
	else
		file = fopen (filename, "w");
	if (!file) {
		cli_outfunf ("ascii export, unable to open \"%s\"", filename);
		return node;
	}
	startlevel = nodes_left (node);

	tnode = node;

	while ((tnode != 0) & (nodes_left (tnode) >= startlevel)) {
		level = nodes_left (tnode) - startlevel;
		flags = node_getflags (tnode);
		cdata = fixnullstring (node_get (tnode, TEXT));
		ascii_export_node (file, level, flags, cdata);

		tnode = node_recurse (tnode);
	}

	if (file != stdout)
		fclose (file);

	cli_outfunf ("ascii export, wrote output to \"%s\"", filename);

	return node;
}


/*
!init_file_ascii();
*/
void init_file_ascii ()
{
	cli_add_command ("export_ascii", export_ascii, "<filename>");
	cli_add_command ("import_ascii", import_ascii, "<filename>");
	cli_add_int ("ascii_margin", &ascii_margin,
				 "the margin that ascii export wraps at (-1=no wrap)");
}
