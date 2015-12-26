/*
 * cli_tokenize.c -- tokenizer for libcli
 *
 * Copyright (C) 2003 Øyvind Kolås <pippin@users.sourceforge.net>
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

enum actions{
	a_nil    =0,		/* do nothing, (just change state) */
	a_eat    =1<<1,	/* increment input position  */
	a_store  =1<<2,	/* increment length of string,  append char to string */
	a_new	 =1<<3	/* increment number of strings, clear output string */
	,
	a_varstore=1<<4,	/* add char to variable name */
	a_varclear=1<<5,    /* clear variable name */
	a_varinsert=1<<6    /* retrieve variable by name, and append to string */
	,
	a_hexstore1=1<<7,	/* store first char of hexsequence */
	a_hexstore2=1<<8,	/* store second char of hexsequence */
	a_hexclear=1<<9,	/* clear stored hexsequence */
	a_hexinsert=1<<10	/* append hexvalue to string */
};

enum tokenize_string_states{
	s_null=0,
	s_start, 	
	s_quo,	
	s_delimit,
	s_wpp,
	s_wescape,
	s_waddchar,
	s_wvar,
	s_qpp,
	s_qescape,
	s_qaddchar,
	s_qvar,
	s_app,
	s_aescape,
	s_end
};

static char *c_ws=" \n\r\t";	/* white space charstring */

typedef struct {
	int state;				/* the state we're in */
	char *chars;			/* chars that this rule applies to, NULL if range is to be used*/
	unsigned char r_start;  /* start of range this rule applies to */
	unsigned char r_end;	/* end of range this rule applies to */
	int action;				/* action to be when performing rule */
	unsigned char storechar; /* action is store and this is non null, store this instead of input */
	int next_state;			/* the state we should change to */
} rule_entry;

#define max_rules 16			/* maximum number of rules for a state */

#ifdef TESTit
#define cli_getstring dummy_get_variable
static char * dummy_get_variable(char *name){
	static char *dummy="<variable expansion not implemented yet>";
	return dummy;
}
#endif

#ifndef TESTit
char *cli_getstring(char *variable);
#endif

static rule_entry state_table[s_end][max_rules];

/* adds a rule to the state_table */
static void a(int state, int next_state, char *chars, unsigned char r_start, unsigned char r_end, int action, unsigned char storechar){
	rule_entry *r=&state_table[state][0];
	while(r->state)r++;
	r->state=state;
	r->r_start=r_start;
	r->r_end=r_end;
	if(chars)
		r->chars=strdup(chars);
	r->action=action;
	r->next_state=next_state;
	r->storechar=storechar;
}

/* *INDENT-OFF* */
static void init_state_table(void){
	static int inited=0;
	if(inited)return;
	inited=1;
	memset(state_table,0,sizeof(state_table));
	a(s_start,	s_start,	c_ws,	0,0,		a_eat,			0);
	a(s_start,	s_quo,		NULL,	0,255,		a_nil,			0);
	
	a(s_quo,	s_delimit,	c_ws,	0,0,		a_eat+a_new,	0);
	a(s_quo,	s_qpp,		NULL,	'"','"',	a_eat,			0);
	a(s_quo,	s_app,		NULL,	'\'','\'',	a_eat,			0);
	a(s_quo,	s_end,		NULL,	'\0','\0',	a_nil+a_new,	0);
	a(s_quo,	s_wvar,		NULL,	'$','$',	a_eat+a_varclear,0);
	a(s_quo,	s_wpp,		NULL,	0,255,		a_eat+a_store,	0);

	a(s_delimit,s_delimit,	c_ws,	0,0,		a_eat,			0);
	a(s_delimit,s_end,		NULL,	'\0','\0',	a_nil,			0);
	a(s_delimit,s_quo,		NULL,	0,255,		a_nil,			0);

	a(s_wpp,	s_wescape,	NULL,	'\\','\\',	a_eat,			0);
	a(s_wpp,	s_quo,		c_ws,	0,0,		a_nil,			0);
	a(s_wpp,	s_quo,		NULL,	'\0','\0',	a_nil,			0);
	a(s_wpp,	s_wvar,		NULL,	'$','$',	a_eat+a_varclear,0);
	a(s_wpp,	s_wpp,		NULL,	0,255,		a_eat+a_store,	0);
	
	a(s_wvar,	s_wvar,		NULL,	'a','z',	a_varstore+a_eat,0);
	a(s_wvar,	s_wvar,		NULL,	'A','Z',	a_varstore+a_eat,0);
	a(s_wvar,	s_wvar,		NULL,	'0','9',	a_varstore+a_eat,0);
	a(s_wvar,	s_wvar,		NULL,	'_','_',	a_varstore+a_eat,0);
	a(s_wvar,	s_wpp,		NULL,	0,255,		a_varinsert,	0);
	
	a(s_wescape, s_quo,	NULL,	'\0','\0',		a_nil,			0);
	a(s_wescape, s_wpp,	NULL,	'n','n',		a_eat+a_store,	'\n');
	a(s_wescape, s_wpp,	NULL,	'r','r',		a_eat+a_store,	'\r');
	a(s_wescape, s_wpp,	NULL,	't','t',		a_eat+a_store,	'\t');
	a(s_wescape, s_wpp,	NULL,	0,255,			a_eat+a_store,	0);
	
	a(s_qpp,	s_quo,	NULL,	'"','"',	a_eat,			0);
	a(s_qpp,	s_qescape,	NULL,	'\\','\\',	a_eat,			0);
	a(s_qpp,	s_quo,		NULL,	'\0','\0',	a_nil,			0);
	a(s_qpp,	s_qvar,		NULL,	'$','$',	a_eat+a_varclear,0);
	a(s_qpp,	s_qpp,		NULL,	0,255,		a_eat+a_store,	0);

	a(s_qvar,	s_qvar,		NULL,	'a','z',	a_varstore+a_eat,0);
	a(s_qvar,	s_qvar,		NULL,	'A','Z',	a_varstore+a_eat,0);
	a(s_qvar,	s_qvar,		NULL,	'0','9',	a_varstore+a_eat,0);
	a(s_qvar,	s_qvar,		NULL,	'_','_',	a_varstore+a_eat,0);
	a(s_qvar,	s_qpp,		NULL,	0,255,		a_varinsert,	0);

	a(s_qescape, s_quo,	NULL,	'\0','\0',		a_nil,			0);
	a(s_qescape, s_qpp,	NULL,	'n','n',		a_eat+a_store,	'\n');
	a(s_qescape, s_qpp,	NULL,	'r','r',		a_eat+a_store,	'\r');
	a(s_qescape, s_qpp,	NULL,	't','t',		a_eat+a_store,	'\t');
	a(s_qescape, s_qpp,	NULL,	0,255,			a_eat+a_store,	0);

	a(s_app,	s_quo,	NULL,	'\'','\'',	a_eat,			0);
	a(s_app,	s_aescape,	NULL,	'\\','\\',	a_eat,			0);
	a(s_app,	s_quo,		NULL,	'\0','\0',	a_nil,			0);
	a(s_app,	s_app,		NULL,	0,255,		a_eat+a_store,	0);

	a(s_aescape, s_quo,	NULL,	'\0','\0',		a_nil,			0);
	a(s_aescape, s_app,	NULL,	'n','n',		a_eat+a_store,	'\n');
	a(s_aescape, s_app,	NULL,	'r','r',		a_eat+a_store,	'\r');
	a(s_aescape, s_app,	NULL,	't','t',		a_eat+a_store,	'\t');
	a(s_aescape, s_app,	NULL,	0,255,			a_eat+a_store,	0);

}
/* *INDENT-ON* */


static int is_oneof (char c, char *chars)
{
	while (*chars) {
		if (c == *chars)
			return 1;
		chars++;
	}
	return 0;
}

#define MAX_VARNAMELENGTH 128

char **argv_tokenize(char *input_string){
	char **argv=NULL;
	char *output;
	unsigned char varname[MAX_VARNAMELENGTH]="";
	int varname_pos=0;
	
	init_state_table();
		
	/* first we calculate and allocate space for the argv structure */
	{
		int state=s_start;
		int tokens=0;
		int total_length=0;
		unsigned char *input=(unsigned char*)input_string;
		while(state!=s_end ){
			rule_entry *r=&state_table[state][0];

			while(r->state){
				if( r->chars){
					if(is_oneof(*input,r->chars))
						break;
				} else {
					if ((*input >= r->r_start) && (*input <= r->r_end) )
						break;
				}
				r++;
			}
			
			if(r->action & a_varclear){
				varname_pos=0;
				varname[varname_pos]=0;
			}
			if( (r->action & a_varstore) && varname_pos<MAX_VARNAMELENGTH-1){
				varname[varname_pos++]=*input;
				varname[varname_pos]=0;
			}
			if(r->action & a_varinsert){
				total_length+=strlen(cli_getstring(varname));
				varname_pos=0;
				varname[varname_pos]=0;
			}
			if(r->action & a_store)
				total_length++;
			if(r->action & a_eat)
				input++;
			if(r->action & a_new)
				tokens++;

			state=r->next_state;
		}
		
		argv=malloc(	 sizeof(char*)*(tokens+1)            /* pointers and terminating NULL */
							+sizeof(char)*(total_length+tokens)); /* character data and NUL for all strings */
		memset(argv, 0, sizeof(char*)*(tokens+1)+sizeof(char)*(total_length+tokens));

		output=(char *)argv + sizeof(char*)*(tokens+1);
		argv[0]=output;
	}

	/* then we make a second pass and actually fill up the argv structure */	
	{
		int state=s_start;
		int tokens=0;
		unsigned char *input=(unsigned char*)input_string;
		while(state!=s_end ){
			rule_entry *r=&state_table[state][0];
			while(r->state){
				if( r->chars){
					if(is_oneof(*input,r->chars))
						break;
				} else {
					if ((*input >= r->r_start) && (*input <= r->r_end) )
						break;
				}
				r++;
			}
			

			if(r->action & a_varclear){
				varname_pos=0;
				varname[varname_pos]=0;
			}
			if( (r->action & a_varstore) && varname_pos<MAX_VARNAMELENGTH-1){
				varname[varname_pos++]=*input;
				varname[varname_pos]=0;
			}
			if(r->action & a_varinsert){
				strcpy(output,cli_getstring(varname));
				output+=strlen(cli_getstring(varname));
			}
			if(r->action & a_store){
				if(r->storechar){
					*(output++)=r->storechar;
				} else {
					*(output++)=*input;
				}
			}
			if(r->action & a_eat)
				input++;
			if(r->action & a_new){
				*output=0;
				argv[++tokens]=++output;
			}
			state=r->next_state;
		}
		argv[tokens]=NULL;	
	}
	
	return argv;
}

int argc_of_argv(char **argv){
	int count=0;
	if(!argv)return 0;
	while(*(argv++))count++;
	return count;
}

#ifdef TESTit

int main(int argc, char **argv){
	char *input="hello$foo world";
	char **largv;
	largv=argv_tokenize(input);
	printf("%i\n",argc_of_argv(largv));

	{char **tlargv=largv;
	while(*tlargv){
		printf("[%s]",*tlargv);
		tlargv++;
	}
	}
	
	printf("\n");
	free(largv);
	return 0;
}
#endif
