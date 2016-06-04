/*
 * tree_sort.c -- function to sort a level of nodes
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

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"
#include "cli.h"

static int cmp_random (Node *b, Node *a)
{
	return (random () % 3) - 1;
}

static int is_done(Node *n){
	char *r=node_get(n,"done");
	if(!r)return 0;
	if(!strcmp(r,"yes"))
		return 1;
	return 0;
}

static int cmp_todo (Node *a, Node *b)
{
	if (!(a->flags) && !(b->flags))
		return (strcasecmp
				(fixnullstring (node_get (a, TEXT)),
				 fixnullstring (node_get (b, TEXT))));

	if (node_get(a,"done") && !node_get(b,"done"))
		return 1;				/*  all todos at top */
	if (!node_get(a,"done") && node_get(b,"done"))
		return -1;

	if (node_get(a,"done") && node_get(b, "done")) {
		if (!is_done(a) && is_done(b) )
			return 1;		/* not done */
		if (is_done(a) && !is_done(b) )
			return -1;
	}
	return (strcasecmp
			(fixnullstring (node_get (a, TEXT)),
			 fixnullstring (node_get (b, TEXT))));
}

static Node *merge (Node *nodeA, Node *nodeB, int (*cmp) (Node *a, Node *b))
{
	Node *thead, *tnode;

	if (!nodeA)
		return nodeB;
	if (!nodeB)
		return nodeA;

	/* first move the smallest of the head nodes to our head */
	if (cmp (nodeA, nodeB) <= 0) {	/* a is smaller than or equal to b */
		thead = nodeA;
		nodeA = nodeA->down;
		thead->down = NULL;
	} else {					/* b is smaller than or equal to a */
		thead = nodeB;
		nodeB = nodeB->down;
		thead->down = NULL;
	}

	tnode = thead;

	/* merge while we get data from both lists */
	while (nodeA && nodeB) {
		if (cmp (nodeA, nodeB) <= 0) {	/* a is smaller than or equal to b */
			tnode->down = nodeA;
			nodeA->up = tnode;
			tnode = nodeA;
			nodeA = nodeA->down;
		} else {				/* b is smaller than or equal to a */
			tnode->down = nodeB;
			nodeB->up = tnode;
			tnode = nodeB;
			nodeB = nodeB->down;
		}
	}

	/* add remainder of remaining list */
	if (nodeA) {
		tnode->down = nodeA;
		nodeA->up = tnode;
	} else if (nodeB) {
		tnode->down = nodeB;
		nodeB->up = tnode;
	} else {
		tnode->down = NULL;
	}

	return thead;
}

/* 
*/
static Node *node_mergesort (Node *head, int size, int (*cmp) (Node *a, Node *b))
{

	if (size == 1) {
		return head;
	} else {
		Node *top = head, *bottom = head;
		int topsize = size / 2, bottomsize = topsize + (size % 2);
		int j = topsize;

		while (j--)
			bottom = node_down (bottom);

		node_up (bottom)->down = NULL;
		bottom->up = NULL;

		top = node_mergesort (top, topsize, cmp);
		bottom = node_mergesort (bottom, bottomsize, cmp);

		return merge (top, bottom, cmp);
	}
}

static int cmp_descending(Node *a,Node *b){
	return cmp_todo(b,a);
}

static void* sort_cmd (int argc, char **argv, void *data)
{
	Node *pos = (Node *) data;
	int (*cmp) (Node *a, Node *b)=cmp_todo;

	if(argc>1){
		if(!strcmp(argv[1],"-a"))cmp=cmp_todo;
		else if(!strcmp(argv[1],"-d"))cmp=cmp_descending;
		else if(!strcmp(argv[1],"-r"))cmp=cmp_random;
	}

	node_mergesort (node_top (pos), nodes_down (node_top (pos)) + 1, cmp);
	if (node_left (pos))
		node_left (pos)->right = node_top (pos);
	return pos;
}

/*
!init_sort();
*/
void init_sort ()
{
	cli_add_command ("sort", sort_cmd, "[-r|-a|-d]");
	cli_add_help ("sort",
		"Sorts the siblings of the currently selected node. The sort type can be specified: -a=ascending; -d=descending; -r=random(shuffle).");
}
