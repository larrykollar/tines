/*
 * ui_cli.c -- the glue that binds various modules into the interpreter
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

#include "tree.h"
#include "file.h"
#include "path.h"
#include "prefs.h"
#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_cli.h"

/* strips the ending node off a path */
static char *path_strip (char *path)
{				/* FIXME add double // escaping when node2path gets it */

	long j = strlen (path);

	while (j > 0) {
		if (path[j - 1] == '/') {
			path[j] = 0;
			return path;
		}
		j--;
	}

	return path;
}

static long add (int argc, char **argv, long *data)
{
	Node *pos = (Node *) data;
	Node *tnode;

	if(argc==1){
		cli_outfunf("usage: %s <new entry>",argv[0]);
		return 0;
	}

	if (argc==2) {
		cli_outfun ("empty node added\n");
	}

	tnode = node_insert_down (node_bottom (pos));
	node_set (tnode, TEXT, argv[1]);
	return (long) pos;
}

static long addc (int argc, char **argv, long *data)
{
	Node *pos = (Node *) data;
	Node *tnode;

	if(argc==1){
		cli_outfunf("usage: %s <entry> [new subentry]",argv[0]);
		return 0;
	}

	tnode = node_exact_match (argv[1], pos);
	if (!tnode) {
		cli_outfun ("specified parent not found");
		return (long) pos;
	}

	if (node_right (tnode)) {
		tnode=node_bottom(tnode);
	} else {
		tnode=node_insert_right(tnode);
	}

	if(argc==2)
		node_set (tnode, TEXT, "");
	else
		node_set (tnode, TEXT, argv[2]);

	return (long) pos;
}

static long pwd (int argc, char **argv, long *data)
{
	Node *pos = (Node *) data;

	cli_outfun (path_strip (node2path (pos)));
	cli_outfun ("\n");
	return (long) pos;
}

static long cd (int argc, char **argv, long *data)
{
	Node *pos = (Node *) data;
	Node *tnode = pos;

	if(argc==1){
		return (long)node_root(pos);
	}

	if (!strcmp (argv[1], "..")){
		if (node_left (tnode) != 0)
			return (long) (node_left (tnode));
	}
		

	tnode = path2node (argv[1], pos);
	if (tnode) {
		tnode = node_right (tnode);
	}
	if (!tnode) {
		cli_outfun ("no such node\n");
		return (long) pos;
	}
	return (long) tnode;

	return (long) pos;
}

#include <ctype.h>

static void pre_command (char *commandline)
{
	char *c = commandline;

	if (commandline) {
		while (isspace ((unsigned char)*c))
			c++;
		if (*c == '#')
			commandline[0] = '\0';
		if (*c == '\0')
			commandline[0] = '\0';
	}
}

static long ls (int argc, char **argv, long *data)
{
	Node *pos = (Node *) data;

	Node *tnode;
	int recurse = 0;
	int indicate_sub = 1;
	int startlevel;

	tnode = node_top (pos);

	startlevel = nodes_left (tnode);
	while (tnode) {

		if (recurse) {
			int j;

			for (j = nodes_left (tnode); j > startlevel; j--) {
				printf ("\t");
			}
		}

		cli_outfunf( "%s %s %s",fixnullstring(node_get (tnode, TEXT)),
		indicate_sub?
		node_right(tnode)?"(..)":""
		:"", tnode==pos?"<":""
		
		);

		if (recurse) {
			tnode = node_recurse (tnode);
			if (nodes_left (tnode) < startlevel)
				tnode = 0;
		} else {
			tnode = node_down (tnode);
		}
	}
	return (long) pos;
}

/*
!init_ui_cli();
*/
void init_ui_cli (void)
{
	static int inited = 0;

	if (!inited) {
		inited = 1;
		cli_precmd = pre_command;
		cli_add_command ("add", add, "<string>");
		cli_add_command ("ls", ls, "");
		cli_add_help("ls", "lists the nodes at the current level");

		cli_add_help("add","inserts an new entry at the current position");
		cli_add_command ("addc", addc, "<parent> <string>");
		cli_add_help("addc","inserts a new entry under the node named parent, with the text in string");
		cli_add_command ("cd", cd, "<path>");
		cli_add_command ("pwd", pwd, "");
		cli_add_help ("pwd", "echoes the current path");
	}
}


Node *docmd (Node *pos, const char *commandline)
{
	long ret;
	char *cmdline = strdup (commandline);

	ret = cli_docmd (cmdline, pos);
	free (cmdline);
	return (Node *) ret;
}

Node *docmdf (Node *pos, char *format, ...){
	va_list arglist;
	char buf[128];
	
	va_start( arglist, format );
	vsnprintf(buf,127,format,arglist);
	va_end(arglist);

	buf[127]=0;
	return docmd(pos,buf);
}


extern int quit_hnb; /* from evilloop. */

Node *cli (Node *pos)
{
	char commandline[4096];

	fprintf (stderr,
			 "Welcome to %s %s\ntype ? or help for more information\n",
			 PACKAGE, VERSION);
    
	do {
		fflush (stdout);
		fprintf (stdout, "%s>", path_strip (node2path (pos)));
		fflush (stdout);
		fgets (commandline, 4096, stdin);
		commandline[strlen (commandline) - 1] = 0;
		pos = (Node *) cli_docmd (commandline, pos);
	} while (!quit_hnb);
	return pos;
}
