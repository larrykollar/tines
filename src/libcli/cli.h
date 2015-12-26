/* libcli -- a small commandline interpreter libraray
 * Copyright (C) 2002 Øyvind Kolås
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
/* helper function 

	takes a orig.inal string
	a char array of 40 to put first word in
	and a pointer to a string to store the tail pointer in

	split on spaces and tabs

	cli_split(commandline, char head[40], &tailstring);
*/
#ifndef LIBCLI_H
#define LIBCLI_H

#include <stdarg.h>

void argv_sanity(void);

void cli_split(char *orig, char *head, char **tail);	

void cli_add_help(char *name,char *helptext);

void cli_cleanup(void);

void
cli_add_item (char *name,
		  long *integer, char *string,
		  long (*func) (int argc,char **argv, long *data), char *usage);

#define cli_add_int(name,integer,usage)\
	cli_add_item (name, integer, NULL, NULL, usage)

#define cli_add_string(name,string,usage)\
	cli_add_item (name, NULL, string, NULL, usage)

#define cli_add_command(name,func,usage) \
	cli_add_item(name, NULL, NULL, func, usage)

int cli_load_file(char *filename);

char *cli_complete (const char *commandline);	/* returns a completed commandline */
long cli_docmd (char *commandline, void *data);	/* run commandline */


extern void (*cli_outfun) (char *);	/* the outputting function 
									   assign cli_outfun a void function that takes a string
									   to use something other than printf
									 */
void cli_outfunf( char *format, ... );

#ifdef WIN32
	#define snprintf(a,b,args...) sprintf(a,args)
#endif


extern void (*cli_precmd) (char *);	/* cstuff to run before executing commands */
extern void (*cli_postcmd) (char *);	/* stuff to run after executing commands */

extern void(*cli_unknown) (int,char **,void *); /* handler for unmatched commands */

extern int cli_width;				/* wrap width of output window */

#include "cli_history.h"
#endif /* LIBCLI_H */
