/*
 * search.c -- recursive search for hnb
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

#include <string.h>
#include "tree.h"
#include "prefs.h"
#include "ui.h"
#include "libcli/cli.h"
#include "query.h"

/************** search ************************/

static long search( int argc, char **argv, long *data){
	Node *pos=(Node *)data;
	
	if(argc==2){
		if(strcmp(argv[1],"-b")||strcmp(argv[1],"-f")){
			pos=node_recursive_match( argv[1],pos);
			if (pos == NULL) {
				docmdf (pos, "status 'reached bottom of tree and \\'%s\\' not found'",
						argv[1]);
				return (long) data;
			}			
		}
	} else if(argc>2){
		if(!strcmp(argv[1],"-b")){
			pos=node_backrecursive_match( argv[2],pos);
			if (pos == NULL) {
				docmdf (pos, "status 'reached top of tree and \\'%s\\' not found'",
						argv[2]);
				return (long) data;
			}			
		} else if(!strcmp(argv[1],"-f")){
			pos=node_recursive_match( argv[2],pos);
			if (pos == NULL) {
				docmdf (pos, "status 'reached bottom of tree and \\'%s\\' not found'",
						argv[2]);
				return (long) data;
			}
		}
		return (long)pos;
	} 
	cli_outfunf("usage: %s [-b|-f] <string>",argv[0]);
	return (long)pos;
}

/*
!init_search();
*/
void init_search ()
{
	cli_add_command ("search",search,"[-b|-f] <string>");
	cli_add_help("search","searches either -b(ackwards) or -f(orwards) in the tree for the next entry with a substring matching the specified string");
}
