/*
 * expanded.c -- functions to expand/unexpand nodes
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

#include <string.h>
#include "tree.h"
#include "cli.h"

#include "evilloop.h"
#include "ctype.h"
#include "ui_binding.h"

static void* cmd_expand (int argc,char **argv, void *data)
{
	Node *pos = (Node *) data;
	if(argc==1){
		if(inputbuf[0] && isprint(lastbinding->key)){ /* evil workaround */
			if (lastbinding->key > 31 && lastbinding->key < 255) {	/*  input for buffer */
				inputbuf[strlen (inputbuf) + 1] = 0;
				inputbuf[strlen (inputbuf)] = lastbinding->key;
			}
			return pos;
		}
		node_setflag(pos,F_expanded,1);
	} else if((!strcmp(argv[1],"-a"))||(!strcmp(argv[1],"--all"))){
		Node *tnode = node_root (pos);

		while (tnode) {
			node_setflag(tnode,F_expanded,1);
			tnode = node_recurse (tnode);
		}
		cli_outfun ("expanded all nodes");
	} else if(!strcmp(argv[1],"--subtree")) {
		node_setflag(pos,F_expanded,1);
		if(node_right(pos)) {
			Node *tnode = node_right(pos);

			while (tnode) {
				node_setflag(tnode,F_expanded,1);
				tnode = node_traverse_right_of (pos, tnode);
			}
		}
		cli_outfun ("expanded subtree");
	} else if((!strcmp(argv[1],"-h"))||(!strcmp(argv[1],"--here"))){
		Node *tnode = node_right(pos);
		int lvl = nodes_left(pos);

		node_setflag(pos,F_expanded,1);

		/* Could avoid repeated nodes_left() call with another
		   recurse function that only goes "right" and "down".
		   Probably not significant though. */

		while (tnode && nodes_left(tnode) > lvl) {
			node_setflag(tnode,F_expanded,1);
			tnode = node_recurse (tnode);
		}
		cli_outfun ("expanded all nodes on branch");
	}
	return pos;
}

static void* cmd_collapse (int argc,char **argv, void *data)
{
	Node *pos = (Node *) data;
	if(argc==1){
		if(inputbuf[0] && isprint(lastbinding->key)){ /* evil workaround */
			if (lastbinding->key > 31 && lastbinding->key < 255) {	/*  input for buffer */
				inputbuf[strlen (inputbuf) + 1] = 0;
				inputbuf[strlen (inputbuf)] = lastbinding->key;
			}		
			return pos;
		}
		node_setflag(pos,F_expanded,0);
	} else if((!strcmp(argv[1],"-a"))||(!strcmp(argv[1],"--all"))){
		Node *tnode = node_root (pos);

		while (tnode) {
			node_setflag(tnode,F_expanded,0);
			tnode = node_recurse (tnode);
		}
		cli_outfun ("collapsed all nodes");
	} else if(!strcmp(argv[1],"--subtree")) {
	        node_setflag(pos,F_expanded,0);
		if(node_right(pos)) {
		  Node *tnode = node_right(pos);

		  while (tnode) {
		    node_setflag(tnode,F_expanded,0);
		    tnode = node_traverse_right_of (pos, tnode);
		  }
		}
		cli_outfun ("collapsed subtree");
	}
	
	return pos;
}

/*
!init_expanded();
*/
void init_expanded ()
{
	cli_add_command ("expand", cmd_expand, "[--all|-a|--here|-h|--subtree]");
	cli_add_help ("expand",
				  "Expands the current entry, showing its subentries. If the \
parameter -a|--all is given, expands all entries in the tree. \
If the parameter -h|--here is given, expands all direct children of the \
current entry. If the parameter --subtree is given, expands all descendants \
of the current entry.");
	cli_add_command ("collapse", cmd_collapse, "[--all|-a|--subtree]");
	cli_add_help ("collapse",
				  "Collapses the current entry's subentries. If the \
parameter -a is given, collapses all entries in the tree. If the parameter \
--subtree is given, collapses everything under the current entry.");
}
