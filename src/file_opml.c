/*
 * file_opml.c -- generic xml import/export filters for Tines
 *
 * Copyright (C) 2001-2003 Øyvind Kolås <pippin@users.sourceforge.net>
 * Modified for Tines by Larry Kollar, 2016
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "xml_tok.h"

#include "cli.h"
#include "tree.h"

#include "file.h"
#include "prefs.h"
#include "query.h"
#include "util_string.h"

char *opml_list_expanded( int init, Node *node, int saveall );
int opml_check_expanded( int num, char *explist );


#define indent(count,char)	{int j;for(j=0;j<count;j++)fprintf(file,char);}

/* *INDENT-OFF* */

static char *xmlquote[]={
	"<","&lt;",
	">","&gt;",
	"&","&amp;",
	"\"","&quot;",
	"'","&apos;",
	NULL
};

static char *xmlunquote[]={
	"&lt;","<",
	"&gt;",">",
	"&amp;","&",
	"&quot;","\"",
	"&apos;","'",
	NULL
};

/* *INDENT-ON* */

/* OPML head variables */
char opml_title[128]   = ""; /* title */
char opml_created[36]  = ""; /* dateCreated */
char opml_mod[36]      = ""; /* dateModified */
char opml_owner[128]   = ""; /* ownerName */
char opml_email[256]   = ""; /* ownerEmail */
char opml_exp[1024]    = ""; /* expansionState */
char opml_scroll[128]  = ""; /* vertScrollState */
char opml_top[10]      = "";  /* windowTop */
char opml_left[10]     = "";  /* windowLeft */
char opml_bot[10]      = ""; /* windowBottom */
char opml_right[10]    = ""; /* windowRight */

static void opml_export_nodes (FILE * file, Node *node, int level, int saveall)
{
	int done = -1; /* -1 = no done attribute; 0 = "no"; 1 = "yes" */
	int todo = 0;  /*  1 if type="todo" */
	int text = 0;  /*  1 if type="text" */

	while (node) {
		done = -1;
		todo = 0;
		text = 0;

		fprintf (file, "\n");
		indent (level, "\t");
		fprintf (file, "<outline");

		{Node_AttItem *att=node->attrib;
			/* build our output map on first pass */
/*			while(att) {
				if(!strcmp(att->name, "done") {
					if(!strcmp(att->data, "yes")) {
						done = 1;
					} else {
						done = 0;
					}
				}
				if(!strcmp(att->name, "type")) {
					if(!strcmp(att->data, "text")) text = 1;
					if(!strcmp(att->data, "todo")) todo = 1;
				}
			}
		 */
		 while(att){
		 	char *quoted=string_replace(att->data,xmlquote);
			fprintf (file, " %s=\"%s\"", att->name, quoted);
			free(quoted);
			att=att->next;
		 }
		}

		if (node_right (node)) {
			fprintf (file, ">");

			opml_export_nodes (file, node_right (node), level+1, 0);
			fprintf (file, "\n");
			indent (level, "\t");
			fprintf (file, "</outline>");
		} else {
			fprintf (file, " />");
		}

		/* needs to only export one branch at level 0 */
		if( level || saveall )
			node = node_down (node);
		else
			break;
	}
}

static void* export_opml (int argc, char **argv, void *data)
{
	Node *node = (Node *) data;
	char *filename;
	int saveall;
	FILE *file;
    time_t curtime;
    int exists;

	/* argv[0]: command name (export_opml or export_opml_branch)
	 * argv[1]: file name (optional, default to stdout)
	 * argv[2]: node position (optional, default to 1) */

    /* saveall = 0 if we're only saving the branch */
	saveall = strcmp(argv[0], "export_opml_branch");

    /* set expansionState */
    strcpy( opml_exp, opml_list_expanded(1, node, saveall) );

    filename = fn_expand( argc>=2?argv[1]:"" );
    exists = file_check(filename);

	if (!strcmp (filename, "-") || !strcmp(filename, ""))
		file = stdout;
	else
		file = fopen (filename, "w");

	if (!file) {
		cli_outfunf ("opml export, unable to open \"%s\"", filename);
		return node;
	}

    /* set modification time to right now, other fields if creating */
    time( &curtime );
    ctime_r( &curtime, opml_mod );
    opml_mod[strlen(opml_mod)-1] = '\0'; /* lop off newline */

    if( !exists ) {
        if( opml_created[0] == '\0' ) /* set create time = mod time */
            strcpy( opml_created, opml_mod );
    }
    /* populate other head elements with reasonable defaults if needed */
    if( opml_title[0] == '\0' ) /* set title to text of first node */
        strcpy( opml_title, node_get(node, TEXT) );
    if( opml_owner[0] == '\0' ) /* set owner too */
        strcpy( opml_owner, getenv("USER") );

    strcpy( opml_scroll, argc>=3?argv[2]:"1" ); /* pos = vertScrollState */

    if( opml_top[0]   == '\0' ) strcpy(opml_top,    "20");
    if( opml_left[0]  == '\0' ) strcpy(opml_left,   "20");
    if( opml_bot[0]   == '\0' ) strcpy(opml_bot,   "620");
    if( opml_right[0] == '\0' ) strcpy(opml_right, "460");

	fprintf (file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n" );
    if( prefs.savepos )
        fprintf(file, "<?tines pos=\"%s\"?>\n", opml_scroll );
    fprintf(file,
"<!-- generated by tines %s (https://github.com/larrykollar/tines) -->\n\
<opml version=\"1.0\">\n", VERSION);
	fprintf (file,
	"<head>\n\
		<title>%s</title>\n\
		<dateCreated>%s</dateCreated>\n\
		<dateModified>%s</dateModified>\n\
		<ownerName>%s</ownerName>\n\
		<ownerEmail>%s</ownerEmail>\n\
		<expansionState>%s</expansionState>\n\
		<vertScrollState>%s</vertScrollState>\n\
		<windowTop>%s</windowTop>\n\
		<windowLeft>%s</windowLeft>\n\
		<windowBottom>%s</windowBottom>\n\
		<windowRight>%s</windowRight>\n\
		</head>\n\
	<body>\n",
			opml_title, opml_created, opml_mod, opml_owner, opml_email,
			opml_exp, opml_scroll, opml_top, opml_left, opml_bot, opml_right);

	opml_export_nodes (file, node, 0, saveall);

	fprintf (file, "\n</body>\n</opml>\n");
	if (file != stdout)
		fclose (file);

	cli_outfunf ("opml export, wrote data to \"%s\"", filename);

	return node;
}

static void* import_opml (int argc, char **argv, void *data)
{
	Node *node = (Node *) data;
	char *filename;
	char *rdata;
    char *datastring = NULL;
	int type;
	int in_body = 0;
	int in_head = 0;
	int in_outlineelement = 0;
	int level = -1;
    int maxstrsize = 0;
	xml_tok_state *s;
	import_state_t ist;
    int entrycount = 1;

	Node *tempnode=NULL;
	FILE *file;

	filename = fn_expand( argc==2?argv[1]:"" );

	file = fopen (filename, "r");
	if (!file) {
		cli_outfunf ("opml import, unable to open \"%s\"", filename);
		return node;
	}
	s = xml_tok_init (file);
	init_import (&ist, node);

	while (((type = xml_tok_get (s, &rdata)) != t_eof) && (type != t_error)) {
		if (type == t_error) { /* does this code ever get run? */
			cli_outfunf ("opml import error, parsing '%s', line:%i %s", filename,
						 s->line_no, rdata);
			fclose (file);
			return node;
		}
        if (type == t_tag && !strcmp (rdata, "head")) {
            in_head = 1;
            continue;
        }
        if( in_head ) {
            if (type == t_tag && !strcmp (rdata, "title")) {
                datastring = opml_title;
                maxstrsize = 128;
                continue;
            }
            if (type == t_tag && !strcmp (rdata, "dateCreated")) {
                datastring = opml_created;
                maxstrsize = 36;
                continue;
            }
            if (type == t_tag && !strcmp (rdata, "dateModified")) {
                datastring = opml_mod;
                maxstrsize = 36;
                continue;
            }
            if (type == t_tag && !strcmp (rdata, "ownerName")) {
                datastring = opml_owner;
                maxstrsize = 128;
                continue;
            }
            if (type == t_tag && !strcmp (rdata, "ownerEmail")) {
                datastring = opml_email;
                maxstrsize = 256;
                continue;
            }
            if (type == t_tag && !strcmp (rdata, "expansionState")) {
                datastring = opml_exp;
                maxstrsize = 1024;
                continue;
            }
            if (type == t_tag && !strcmp (rdata, "vertScrollState")) {
                datastring = opml_scroll;
                maxstrsize = 128;
                continue;
            }
            if (type == t_tag && !strcmp (rdata, "windowTop")) {
                datastring = opml_top;
                maxstrsize = 10;
                continue;
            }
            if (type == t_tag && !strcmp (rdata, "windowLeft")) {
                datastring = opml_left;
                maxstrsize = 10;
                continue;
            }
            if (type == t_tag && !strcmp (rdata, "windowBottom")) {
                datastring = opml_bot;
                maxstrsize = 10;
                continue;
            }
            if (type == t_tag && !strcmp (rdata, "windowRight")) {
                datastring = opml_right;
                maxstrsize = 10;
                continue;
            }
            if (type == t_word && datastring != NULL) {
                if( strlen(datastring)+strlen(rdata) >= maxstrsize ) {
                    cli_outfunf( "opml import, warning: OPML data string too long: %s", datastring );
                } else {
                    strcat(datastring, rdata);
                }
                continue;
            }
            if (type == t_whitespace && datastring != NULL) {
                if( strlen(datastring) < maxstrsize-2 )
                    strcat(datastring, " ");
                continue;
            }
            if (type == t_entity && datastring != NULL ) {
                char junk[60];
                sprintf( junk, "&%s;", rdata );
                if( strlen(datastring)+strlen(junk) >= maxstrsize ) {
                    cli_outfunf( "opml import, warning: OPML data string too long: %s", datastring );
                } else {
                    strcat(datastring, junk);
                }
                continue;
            }
            if ( (type == t_closetag || type == t_closeemptytag) ) {
                if( !strcmp(rdata, "head") )
                    in_head = 0;
                datastring = NULL;
                continue;
            }
        }
        if (type == t_tag && !strcmp (rdata, "body")) {
            in_body = 1;
            continue;
        }

        if (in_body) {
			if (type == t_tag && !strcmp (rdata, "outline")) {
				level++;
				in_outlineelement = 1;
				tempnode=node_new();
                if( opml_check_expanded(entrycount, opml_exp) ) {
                    node_setflag( tempnode, F_expanded, 1 );
                }
                ++entrycount;
				continue;
			}
			if (in_outlineelement && type == t_att){
				char *att_name=strdup(rdata);
				char *unquoted;
				if(xml_tok_get(s,&rdata)!=t_val){
					cli_outfunf("import_opml: OPML violation: attribute '%s' has no value, line %d", att_name, s->line_no);
				};
				unquoted=string_replace(rdata,xmlunquote);

				node_set(tempnode,att_name,unquoted);
				free(unquoted);
				free(att_name);
				continue;
			}
			if ((type == t_endtag || type == t_closeemptytag)
				&& !strcmp (rdata, "outline")) {
				in_outlineelement = 0;
				import_node(&ist, level, tempnode);	/* will free tempnode */
				tempnode=NULL;
			}
			if ((type == t_closetag || type == t_closeemptytag)
				&& !strcmp (rdata, "outline")) {
				level--;
				continue;
			}
            if ( (type == t_closetag || type == t_closeemptytag)
                    && !strcmp(rdata, "body") ) {
                in_body = 0;
                continue;
            }
	   }
    }

	if (node_getflag (node, F_temp))
		node = node_remove (node);	/* remove temporary node, if tree was empty */

	cli_outfunf ("opml import - imported \"%s\" %i lines", filename, s->line_no);
	xml_tok_cleanup (s);
	return node;
}

/* Build list of expanded nodes for expansionState */
char *opml_list_expanded( int init, Node *node, int saveall ) {

    static char explist[1025] = "";
    char entry[10];
    static int explen = 0;
    static int opml_nodenum = 0;

    if( init ) { /* need to reset these for 2nd export */
        explist[0] = '\0';
        explen = 0;
        opml_nodenum = 0;
    }

    while( node ) {
        ++opml_nodenum;
        if (node_getflag(node,F_expanded)) { /* add node number to list */
            sprintf( entry, ",%d", opml_nodenum );
            explen += strlen( entry );
            if( explen < 1025 )
                strcat( explist, entry );
        }
        if (node_right (node)) { /* recurse into sub-entries */
			opml_list_expanded( 0, node_right(node), 1 );
		}

		/* needs to only export one branch at level 0 */
		if( saveall )
			node = node_down (node);
		else
			break;
    }
    return (char *)explist+1; /* skip initial comma */
}

/* returns 1 if the number appears in the expansionState list */
int opml_check_expanded( int num, char *explist ) {
    char entry[10];
    char fullsize[1026];

    sprintf( fullsize, ",%s,", explist ); /* put commas on either end of list */
    sprintf( entry, ",%d,", num ); /* now we only have to search once */

    if( strstr(fullsize, entry) )
        return 1;
    else
        return 0;
}

/*
!init_file_opml();
*/
void init_file_opml ()
{
	cli_add_command ("export_opml", export_opml, "<filename>");
	cli_add_command ("export_opml_branch", export_opml, "<filename>");
	cli_add_help ("export_opml",
		"Exports the current entry, following siblings, and child entries in OPML format.");
	cli_add_help ("export_opml_branch",
		"Exports the current entry and child entries in OPML format.");
	cli_add_command ("import_opml", import_opml, "<filename>");
	cli_add_help ("import_opml",
		"Imports the specified OPML file below the current entry.");

    cli_add_string ("opml_title", opml_title, "The outline title, from OPML metadata.");
    cli_add_string ("opml_created", opml_created, "The outline creation date.");
    cli_add_string ("opml_mod", opml_mod, "The outline modification date.");
    cli_add_string ("opml_owner", opml_owner, "The name of the creator.");
    cli_add_string ("opml_email", opml_email, "The email address of the creator.");
    cli_add_string ("opml_exp", opml_exp, "List of expanded nodes.");
    cli_add_string ("opml_scroll", opml_scroll, "Position when last closed.");
    cli_add_string ("opml_top", opml_top, "Position of top of window. Tines does not use this.");
    cli_add_string ("opml_left", opml_left, "Position of left side of window. Tines does not use this.");
    cli_add_string ("opml_bot", opml_bot, "Position of bottom of window. Tines does not use this.");
    cli_add_string ("opml_right", opml_right, "Position of right side of window. Tines does not use this.");
}
