/*
 * macro.c -- adding macro defining capabilities to libcli
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

#define MAX_MACRO_LINES 512
#include "tree.h"
#include <stdlib.h>
#include "cli.h"
#include "ui_cli.h"
#include <ctype.h>
#include <string.h>

static void (*precmd_backup) (char *)=NULL;	

typedef struct MacroT{
	char *name;
	char **commands;
	struct MacroT *next;
} MacroT;

static MacroT *macro = NULL;
static char **cur_cmd = NULL;

static void macro_pre_command (char *commandline)
{
	char *c = commandline;

	if (commandline) {
		while (isspace ((unsigned char)*c))
			c++;
		if (*c == '#')
			commandline[0] = '\0';
		if (*c == '\0')
			commandline[0] = '\0';
		if(!strcmp(c,"end")){
			*cur_cmd=NULL;
			cli_precmd=precmd_backup;
			commandline[0]= '\0';
		} else {
			*cur_cmd=strdup(c);
			cur_cmd++;
			commandline[0]= '\0'; /* don't let the cli run the macro whilst we're doing this */
		}
	}
}

static MacroT *lookup_macro(char *name){
	MacroT *tmacro=macro;
	while(tmacro){
		if(!strcmp(name,tmacro->name))
			return tmacro;
		tmacro=tmacro->next;
	}
	return NULL;
}

static Node *do_macro(MacroT *macro, Node *pos){
	char **curcmd=macro->commands;
	while(*curcmd){
		pos=docmd(pos,*curcmd);
		curcmd++;
	}
	
	return pos;
}

static void* cmd_macro (int argc, char **argv, void *data)
{
	Node *pos=(Node *)data;
	if(argc==1){
		cli_outfunf("usage: %s ....",argv[0]);
	} else if(argc==2){
		MacroT *tmacro=lookup_macro(argv[1]);
		if(tmacro){
			return do_macro(tmacro,pos);
		} else {
			cli_outfunf("no such macro defined '%s'",argv[1]);
		}
		/* run macro */
	} else if(argc==3) {
		if(!strcmp(argv[1],"define") ){
			if(lookup_macro(argv[2])){
				cli_outfunf("error macro %s already exist,.. this might turn out badly,.. " ,argv[2]);
				return pos;
			} else {
				MacroT *tmacro;
				if(!macro){
					macro=calloc(1,sizeof(MacroT));
					tmacro=macro;
				} else {
					tmacro=macro;
					while(tmacro->next)
						tmacro=tmacro->next;
					tmacro->next=calloc(1,sizeof(MacroT));
					tmacro=tmacro->next;
				}
				tmacro->name=strdup(argv[2]);
				tmacro->commands=calloc(sizeof(char *),MAX_MACRO_LINES);
				cur_cmd=(tmacro->commands);
				precmd_backup=cli_precmd;
				cli_precmd=macro_pre_command;
			}
			
		}
	}
	return pos;
}


/*
!init_cli_macro();
*/
void init_cli_macro (void)
{
		cli_add_command ("macro", cmd_macro, "");
		cli_add_help ("macro", "");
}
