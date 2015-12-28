
/*
 * node.c -- definition of hnb's nodes
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
#include <stdlib.h>
#include "node.h"
#include "assert.h"


Node *node_new ()
{
	Node *node = (Node *) malloc (sizeof (Node));
	memset (node, 0, (sizeof (Node)));
/*	node->flags=1<<3;*/
	return node;
}

Node *node_duplicate (Node *node)
{
	Node *newnode;
	Node_AttItem *att;

	assert (node);
	if (!node)
		return NULL;
	newnode = (Node *) malloc (sizeof (Node));
	if (!newnode)
		return NULL;
	memcpy (newnode, node, sizeof (Node));

	newnode->attrib = NULL;
	att = node->attrib;
	while (att) {
		node_set (newnode, att->name, att->data);
		att = att->next;
	}

	return newnode;
}

void node_free (Node *node)
{
	while (node->attrib) {
		node_unset (node, node->attrib->name);
	}
	free (node);
}


/* returns pointer to character data of attribute NULL if it isn't
   set */
char *node_get (Node *node,char *name)
{
	Node_AttItem *att;
	if(!node)
		return NULL;
	att = node->attrib;
	while (att) {
		if (!strcmp (att->name, name)) {
			return att->data;
		}
		att = att->next;
	}
	return NULL;
}

/* sets the named attribute to the value of *data
*/
void node_set (Node *node, char *name, char *data)
{
	Node_AttItem *att;

	att = node->attrib;
	while (att) {
		if (!strcmp (att->name, name)) {
			free (att->data);
			att->data = strdup (data);
			return;
		}
		att = att->next;
	}

	/* did not find the requested att, inserting new one, (stack wise) */

	att = (Node_AttItem *) malloc (sizeof (Node_AttItem));
	att->next = node->attrib;
	att->data = strdup (data);
	att->name = strdup (name);
	node->attrib = att;

	return;
}

void node_unset (Node *node, char *name)
{
	Node_AttItem *att;
	Node_AttItem **prev;


	prev = &(node->attrib);
	att = node->attrib;
	while (att) {
		if (!strcmp (att->name, name)) {
			free (att->data);
			free (att->name);
			*prev = att->next;
			free (att);
			return;
		}
		prev = &(att->next);
		att = att->next;
	}

	/* no such node */
	return;
}

/* FIXME: the rest of this file isn't actually used for anything but debugging yet,..*/

#include "cli.h"
#include <stdio.h>


long cmd_att_set (int argc, char **argv, long *data)
{
	Node *pos = (Node *) data;

	if(argc!=3){
		cli_outfunf("usage: %s <attribute> <value>",argv[0]);
		return (long) pos;
	}
		
	node_set (pos, argv[1], argv[2]);
	return (long) pos;
}

long cmd_att_get (int argc, char **argv, long *data)
{
	Node *pos = (Node *) data;
	char *cdata;
	
	if(argc!=2){
		cli_outfunf("usage: %s <attribute>",argv[0]);
		return (long) pos;
	}
			
	cdata = node_get (pos, argv[1]);

	if (cdata)
		cli_outfun (cdata);
	return (long) pos;
}

long cmd_att_clear (int argc, char **argv, long *data)
{
	Node *pos = (Node *) data;
	if(argc!=2){
		cli_outfunf("usage: %s <attribute>",argv[0]);
		return (long) pos;
	}
	node_unset (pos, argv[1]);
	return (long) pos;
}

long cmd_att_list (int argc, char **argv, long *data)
{
	Node_AttItem *att;
	Node *pos = (Node *) data;

	att = pos->attrib;
	while (att) {
		cli_outfunf ("%s: [%s]", att->name, att->data);
		att = att->next;
	}
	return (long) pos;
}

/*
!init_nodetest();
*/
void init_nodetest ()
{
	cli_add_command ("att_set", cmd_att_set, "<attribute> <value>");
	cli_add_command ("att_get", cmd_att_get, "<attribute>");
	cli_add_command ("att_clear", cmd_att_clear, "<attribute>");
	cli_add_command ("att_list", cmd_att_list, "");
	cli_add_help( "att_set", "Sets an attribute for the current entry." );
	cli_add_help( "att_get", "Returns the specified attribute value for the current entry." );
	cli_add_help( "att_clear", "Removes the specified attribute." );
	cli_add_help( "att_list", "Lists the text and attributes for the current entry." );
}
