/*
 * actions.c -- binding of external applications according to nodedata
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

/**************/
#include "tree.h"
#include "cli.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*static char url[1024]="";
static char address[1024]="";*/
static char web_command[255] = "lynx *";
static char mail_command[255] = "rxvt -rv +sb -e mutt *";

static void* cmd_system(int argc, char **argv, void *data){
	Node *pos=(Node *)data;
	int ui_was_inited = ui_inited;
	if (argc>1) {
		if (ui_was_inited)
			ui_end ();			
		system (argv[1]);
		if (ui_was_inited)
			ui_init ();
	}
	return pos;
}

static int action_node (Node *node)
{
	char cmdline[512] = "";
	char *start = NULL;
	int ui_was_inited = ui_inited;

	if (!strncmp ("exec ", fixnullstring (node_get (node, TEXT)), 5)) {
		sprintf (cmdline, "%s > /dev/null 2>&1 &",
				 fixnullstring (node_get (node, TEXT)) + 5);
	} else
		if ((start =
			 strstr (fixnullstring (node_get (node, TEXT)), "http://"))) {
		char url[200];
		char *dest;

		dest = url;
		while (*start && !isspace ((unsigned char)*start)) {
			*dest = *start;
			dest++;
			start++;
		}
		*dest = 0;
		{
			char *cs = web_command;
			char *cd = cmdline;

			while (*cs) {
				if (*cs == '*') {
					strcat (cd, url);
					strcat (cd, cs + 1);
					cli_outfunf ("shelling out: %s", cmdline);
					//strcat(cd,"> /dev/null 2>&1 &");
					break;
				} else {
					*cd = *cs;
					*(++cd) = '\0';
				}
				cs++;
			}
		}
	} else if ((start = strchr (fixnullstring (node_get (node, TEXT)), '@'))) {
		char mail_address[200];
		char *dest;

		dest = mail_address;

		while (start > fixnullstring (node_get (node, TEXT))
			   && !isspace ((unsigned char)*start))
			start--;
		while (*start && !isspace (*start)) {
			*dest = *start;
			dest++;
			start++;
		}
		*dest = 0;
		{
			char *cs = mail_command;
			char *cd = cmdline;

			while (*cs) {
				if (*cs == '*') {
					strcat (cd, mail_address);
					strcat (cd, cs + 1);
					cli_outfunf ("shelling out: %s", cmdline);
					//strcat(cd,"> /dev/null 2>&1 &");
					break;
				} else {
					*cd = *cs;
					*(++cd) = '\0';
				}
				cs++;
			}
		}
	}

	if (cmdline[0]) {
		if (ui_was_inited)
			ui_end ();
		system (cmdline);
		if (ui_was_inited)
			ui_init ();
		return 0;
	}
	return -1;
}

/*
 * this is simplistic approach,.. should perhaps have another one that checks for 
 * url/email address substring,.. and launches an app based on that?
 *
 */
static void* cmd_action (int argc, char **argv, void *data)
{
	Node *pos = (Node *) data;
	Node *node = node_right (pos);

	while (node) {
		if (!action_node (node))
			return pos;
		node = node_down (node);
	}
	if (!action_node (pos)) {
		return pos;
	} else {
		cli_outfunf ("nothing to do");
		return pos;
	}

	/***
	***/
}

/*
!init_exec_cmd();
*/
void init_exec_cmd ()
{
	cli_add_command ("system", cmd_system, "<commandline>");

	cli_add_command ("action", cmd_action, "");
	cli_add_help ("action",
				  "executes an external shell according to node content, it first checks the direct children \
then the current node. If it contains an http:// url, the command described in browser_command is executed, if it \
contains a '@' the characters surrounding it is interpreted as an mail address and the mail_command is executed \
and if the data starts with 'exec' the rest of the data is executed verbatim.");

	cli_add_string ("web_command", web_command,
					"Command executed when opening url's * is subsituted with the url");
	cli_add_string ("mail_command", mail_command,
					"Command executed when sending mail to a mailaddress * is substituted with the address");
/*	cli_add_string("url",url,"url used as parameter for webcommand");
	cli_add_string("address",address,"email address used as parameter for mail_command);
*/}
