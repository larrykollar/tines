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
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cli.h"
#include "cli_tokenize.h"

/*#define HIDE_NULL_HELP
*/
/* TODO:

	allow removal of commands/variables
	scripts? (with simple flow-control?)
*/

static char tempstr[100];

#ifdef WIN32
#define snprintf(a,b,args...) sprintf(a,args)
#endif
/*
	wordwrapping outputting function
*/
static void default_output(char *data){
	#define COLS 78
	char *tbuf=malloc(COLS+1);
	char *word=malloc(COLS+1);
	char *bp=tbuf,
	     *wp=word,
		 *dp=data;
	
	*bp=*wp='\0';
	
	while(1+1==2){
		if(isspace((unsigned char)*dp) || *dp=='\0'){
			if( (bp-tbuf) + (wp-word) +1 < COLS){
				strcpy(bp,word);
				bp+=(wp-word);
				*(bp++)=' ';
				*bp='\0';
				wp=word;
				*wp='\0';
			} else {
				/* printf(tbuf);printf("\n"); */
				printf( "%s\n", tbuf );
				bp=tbuf;
				*bp='\0';
				strcpy(bp,word);
				bp+=(wp-word);
				*(bp++)=' ';
				*bp='\0';
				wp=word;
				*wp='\0';
			}
			if(!*dp)break;
		} else {
			if(wp-word>=COLS){
				printf("%s\n",tbuf);
				printf("%s\n",word);
				wp=word;
			}
			*(wp++)=*dp;
			*wp='\0';
		}
		dp++;
	}
	printf("%s\n",tbuf);
	
	free(word);
	free(tbuf);
}

static void default_unknown_command (int argc, char **argv, void *data){
	cli_outfunf ("Unknown command: '%s' Use '?' to see valid commands.\n",argv[0]);
}

void (*cli_outfun) (char *) = default_output;
void (*cli_precmd) (char *) = NULL;
void (*cli_postcmd) (char *) = NULL;
void (*cli_unknown) (int,char **,void *) = default_unknown_command;
int cli_width = 40;


void cli_outfunf(char *format, ...){
	va_list arglist;
	char buf[128];
	
	va_start( arglist, format );
	vsnprintf(buf,127,format,arglist);
	va_end(arglist);

	buf[127]=0;
	cli_outfun(buf);
}


static  long item_matches (const char *itemname);

typedef struct ItemT {
	char *name;					/* what the user types */
	void* (*func) (int argc, char **argv, void *data);	/* function that is the command */
	long *integer;				/* pointer to integer (set to NULL if string) */
	char *string;				/* pointer to string (set to NULL if integer) */
	char *usage;				/* helptext for this command */
	char *help;
	int flags;
	struct ItemT *next;
} ItemT;

#define is_command(a) (a->func && (a->integer==NULL) && (a->string==NULL))
#define is_variable(a) (!is_command(a))

static ItemT *items = NULL;

void *
cli_add_item (char *name,
		  long *integer, char *string,
		  void* (*func) (int argc, char **argv, void *data), char *usage)
{
	ItemT *titem = items;

	while(titem){
		if(!strcmp(titem->name,name)){
			cli_outfunf ("libcli: attempted to add item '%s' more than once\n", name);
			return (void *)0;
		}
		titem=titem->next;
	}
	titem=items;

	if (!titem) {
		titem = items = malloc (sizeof (ItemT));
		titem->next = NULL;
	} else {
		ItemT *tmp;

		while (titem->next && ((strcmp ((titem->next)->name, name)) < 0)) {
			titem = titem->next;
		}

		tmp = titem->next;
		titem->next = malloc (sizeof (ItemT));
		titem = titem->next;
		titem->next = tmp;
	}

	titem->name = strdup (name);
	titem->func = func;
	titem->integer = integer;
	titem->string = string;
	if(usage)titem->usage = strdup (usage);
	titem->help=strdup("");

	if (strcmp (items->name, titem->name) > 0) {
		ItemT *tmp = items;
		ItemT *tmp_next = titem->next;

		items = titem;
		items->next = tmp;
		items->next->next = tmp_next;
	}
	return (void *)0;
}

void cli_add_help(char *name, char *helptext){
	ItemT *titem = items;

	while (titem) {
		if (!strcmp (name, titem->name)){
				free(titem->help);
				titem->help=strdup(helptext);
				return;
		}
		titem=titem->next;
	}
	cli_outfunf("libcli: attempted to add help for '%s' which is not registered",name);
}


static void* help (int argc,char **argv, void *data);
static void* vars (int argc,char **argv, void *data);

static int inited = 0;

void cli_cleanup(void){
	inited=0;
	cli_outfun  = default_output;
	cli_precmd  = NULL;
	cli_postcmd  = NULL;
	cli_unknown = default_unknown_command;
	cli_width = 40;

	while(items){
		ItemT *titem=items;
		if(items->name)free(items->name);
		if(items->usage)free(items->usage);
		if(items->help)free(items->help);
		items=items->next;
		free(titem);
		titem=NULL;
	}
}

static void init_cli (void)
{
	cli_add_command ("?", help, "Displays this listing.");
	cli_add_command ("show_vars", vars, "Shows all variables and values.");
	cli_add_string  ("temp_str", tempstr, "A temporary variable for use in macros." );
	inited = 1;
}

int cli_calllevel=0;

void *cli_docmd (char *commandline, void *data)
{
	int largc=0;
	char **largv;
	
	ItemT *titem = items;
	void *ret = data;
	cli_calllevel++;

	if (cli_precmd)
		cli_precmd (commandline);

	if (!inited) {
		init_cli ();
		titem = items;
		inited = 1;
	}

	largv=argv_tokenize(commandline);
	if(largv)largc=argc_of_argv(largv);

	if((!largc) || largv[0][0]=='\0' ){
		free(largv);
		return ret;
	}
	
	while (titem) {
		if (!strcmp (largv[0], titem->name)) {
			if (is_command (titem)) {
				ret=titem->func (largc, largv, data);
				
				if (cli_postcmd)
					cli_postcmd (commandline);
				cli_calllevel--;

				free(largv);
				return ret;
			} else if (is_variable (titem)) {
				if (largc==1) {
					if (titem->string) {
						cli_outfunf ("%s\t[%s]\t- %s\n", titem->name,
							  titem->string, titem->usage);
					} else if (titem->integer) {
						cli_outfunf ("%s\t[%i]\t- %s\n", titem->name,
							  *titem->integer, titem->usage);
					} else {
						cli_outfunf ("%s\tis a broken variable\n", titem->name);
					}
				} else {
					if (titem->integer)
						*titem->integer = atoi (largv[1]);
					if (titem->string)
						strcpy (titem->string, largv[1]);
					if (titem->func)
						ret=(void *)titem->func (largc,largv, data);
				}
				if (cli_postcmd)
					cli_postcmd (commandline);
				cli_calllevel--;

				free(largv);				
				return ret;
			}
		}
		titem = titem->next;
	}
	if(cli_unknown)
		cli_unknown(1,&commandline,data);
	if (cli_postcmd)
		cli_postcmd (commandline);
	cli_calllevel--;
	
	free(largv);
	return ret;
}

static char newcommand[100];

static long item_matches (const char *itemname)
{
	int matches = 0;
	ItemT *titem = items;

	while (titem) {
		if (!strncmp (itemname, titem->name, strlen (itemname)))
			matches++;
		titem = titem->next;
	}
	return matches;
}

char *cli_complete (const char *commandline)
{
	long matches = 0;
	char str_matches[4096]="";

	strncpy (newcommand, commandline, 99);
	newcommand[99] = 0;

	if (commandline[0]) {
		matches = item_matches (newcommand);

		if (matches == 1) {
			ItemT *titem = items;

			while (titem) {
				if (!strncmp (newcommand, titem->name, strlen (newcommand))) {
					long pos;

					strcpy (newcommand, titem->name);
					pos = strlen (newcommand);
					newcommand[pos++] = ' ';
					newcommand[pos] = '\0';
					break;
				}
				titem = titem->next;
			}
		} else if (matches > 1) {
			ItemT *titem = items;
			strcpy(str_matches,"matches: ");
			while (titem) {
				if (!strncmp (newcommand, titem->name, strlen (newcommand))) {
					strcat (str_matches,titem->name);
					strcat (str_matches," ");
				}
				titem = titem->next;
			}
			cli_outfun(str_matches);
			while (item_matches (newcommand) == matches) {
				ItemT *titem = items;

				while (titem) {
					long len = strlen (newcommand);

					if (!strncmp (newcommand, titem->name, len)) {

						strcpy (newcommand, titem->name);
						newcommand[len + 1] = '\0';
						if(!strcmp(newcommand,titem->name)){
							return newcommand;
						}
						break;
					}
					titem = titem->next;
				}
			}
			newcommand[strlen (newcommand) - 1] = '\0';
		} else {
			cli_outfunf ("no match");
		}
	}

	return newcommand;
}

/* internal commands */

static void* help (int argc, char **argv, void *data)
{
	int lines = 0;

	if (argc == 1) {		/* show all help */
		ItemT *titem = items;

		cli_outfunf ("available commands:");

		while (titem) {
		#ifdef HIDE_NULL_HELP
			if(titem->usage)
		#endif
			if (is_command (titem)) {
				cli_outfunf ("%14s %s", titem->name, titem->usage);
				++lines;
			}
			if(titem->help[0]) {
				cli_outfun(titem->help);
				++lines;
			}

			titem = titem->next;
		};
	} else {					/* show help for specified command */
		ItemT *titem = items;

		cli_outfunf ("HELP for '%s'", argv[1] );

		while (titem) {
			if (is_command (titem)) {
				if (!strcmp (argv[1], titem->name)) {
					cli_outfunf ("usage: %s %s", titem->name, titem->usage);
					if(titem->help[0])
						cli_outfun(titem->help);
					
					return data;
				}
			}
			titem = titem->next;
		}
		cli_outfunf ("unknown command '%s'", argv[1]);
	}
	return data;
}

static void* vars (int argc, char **argv, void *data)
{
	ItemT *titem = items;

	cli_outfunf ("All variables:");

	while (titem) {
		#ifdef HIDE_NULL_HELP
			if(titem->usage)
		#endif
		if (is_variable (titem)) {
			if (titem->string) {
				cli_outfunf ("%15s [%s]\t- %s", titem->name,
					  titem->string, titem->usage);
			} else if (titem->integer) {
				cli_outfunf ("%15s [%i]\t- %s", titem->name,
					  *titem->integer, titem->usage);
			} else {
				cli_outfunf ("%s\tis a broken variable", titem->name);
			}

		}
		titem = titem->next;
	}

	cli_outfunf ("----------------");
	cli_outfunf ("to change a variable: \"variablename newvalue\"");
	return data;
}

char *cli_getstring(char *variable){
		ItemT *titem = items;
		while (titem) {
			if (is_variable (titem)) {
				if (!strcmp (variable, titem->name)) {
					if(titem->string)
						return(titem->string);
					if(titem->integer)
						return NULL; /* FIXME: use a static buffer perhaps */
				}
			}
			titem = titem->next;
		}
		return "";
}

#include <stdio.h>

int cli_load_file(char *filename){
	char buf[255];
	FILE *file;

	file=fopen(filename,"r");
	if(!file){
		return -1;
	}

	while(fgets(buf,255,file)){
		char *c=strchr(buf,'\n');
		char *t;
		t=buf;
		if(c)*c='\0';
		if(*buf){
			while(*t==' ' || *t=='\t')t++;
			cli_docmd(buf,NULL);
		}
	}
	fclose(file);

	return 0;
}
