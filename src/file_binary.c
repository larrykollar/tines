/*
 * file_binary.c -- binary fileformat, version specific and for internal use only 
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cli.h"
#include "tree.h"

#include "file.h"
#include "prefs.h"
#include "query.h"
#include "util_string.h"

#define char2int(c1,c2,c3,c4) ((c1)+256*(c2)+256*256*(c3)+256*256*256*(c4))

static unsigned int hnb_binary_header = char2int('h','n','b','\n');
static unsigned int hnb_binary_version = 1;

#define indent(count,char)	{int j;for(j=0;j<count;j++)fprintf(file,char);}


static void binary_export_nodes (FILE * file, Node *node, int level)
{
	while (node) {
		int attributes=0;
		{Node_AttItem *att=node->attrib;
	         while(att){
			attributes++;
		 	att=att->next;
		 }
		}
			
		fwrite(&level, sizeof(int), 1, file);
		fwrite(&attributes, sizeof(int), 1, file);

		{Node_AttItem *att=node->attrib;
	 	 while(att){
			 long len=strlen(att->name);
			 fwrite(&len,sizeof(int),1,file);
			 fwrite(att->name,1,len, file);
			 len=strlen(att->data);
			 fwrite(&len,sizeof(int),1,file);
			 fwrite(att->data,1,len, file);
		   att=att->next;
		 }
		}
		
		if (node_right (node)) {
			binary_export_nodes (file, node_right (node), level + 1);
		}
		node = node_down (node);
	}
}

static void* export_binary (int argc, char **argv, void *data)
{
	Node *node = (Node *) data;
	char *filename;
	FILE *file;

	filename = fn_expand( argc>=2?argv[1]:"", 1	);

	if (!strcmp (filename, "-") || !strcmp(filename, ""))
		file = stdout;
	else
		file = fopen (filename, "w");

	if (!file) {
		cli_outfunf ("binary export, unable to open \"%s\"", filename);
		return node;
	}

	fwrite (&hnb_binary_header, 4, 1, file);
	fwrite (&hnb_binary_version, 4, 1, file);

	binary_export_nodes (file, node, 0);

	cli_outfunf ("binary export, wrote data to \"%s\"", filename);

	return node;
}


static void* import_binary (int argc, char **argv, void *data)
{
	Node *node = (Node *) data;
	char *filename;
	import_state_t ist;
	int moredata=1;



	FILE *file;

	filename = fn_expand( argc==2?argv[1]:"", 0	);

	file = fopen (filename, "r");
	if (!file) {
		cli_outfunf ("binary import, unable to open \"%s\"", filename);
		return node;
	}

	{int header,version;
		fread(&header, sizeof(int), 1, file);
		fread(&version, sizeof(int), 1, file);
		if(header!=hnb_binary_header || version!=hnb_binary_version){
			cli_outfunf("binary import, header mismatch");
		}
	}

	init_import(&ist, node);
	
	while(moredata){
		int attributes;
		int level;
		moredata=fread(&level, sizeof(int), 1, file);
		if(!moredata) break;
		fread(&attributes, sizeof(int),1,file);
		if(!moredata) break;
		
		if(level || attributes){
			Node *temp_node=node_new();
			while(attributes){
				int len;
				char *att_name;
				char *att_data;
				fread(&len, sizeof(int),1,file);
				att_name=malloc(len+1);
				fread(att_name,1,len,file);
				att_name[len]='\0';
				fread(&len, sizeof(int),1,file);
				att_data=malloc(len+1);
				fread(att_data,1,len,file);
				att_data[len]='\0';
				node_set(temp_node, att_name, att_data);
				free(att_name);
				free(att_data);
				attributes--;
			}
			import_node(&ist,level,temp_node);
			temp_node=NULL;	
		}
	}

	if(node_getflag(node,F_temp))
		node=node_remove(node);
	cli_outfunf("binary import - imported \"%s\"",filename);
	
	return node;
}

/*
!init_file_binary();
*/
void init_file_binary ()
{
	cli_add_command ("export_binary", export_binary, "<filename>");
	cli_add_help ("export_binary",
		"Exports the current entry, following siblings, and child entries in binary format.");
	cli_add_command ("import_binary", import_binary, "<filename>");
	cli_add_help ("import_binary",
		"Imports a binary-fomatted file below the current entry.");
}
