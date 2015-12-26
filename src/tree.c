/*
 * tree.c -- a general tree structure for use with hnb
 *
 * Copyright (C) 2001,2002 Øyvind Kolås <pippin@users.sourceforge.net>
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


/* there is one a little bit strange thing about
this tree, it have the root at the top but
at the left.. (like the model presented to the 
user) */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"

char TEXT[5] = "text";

Node *node_recurse (Node *node)
{
	if (node_right (node))
		return node_right (node);
	if (node_down (node))
		return node_down (node);

	while (node_left (node)) {
		if (node_down (node_left (node)))
			return node_down (node_left (node));
		node = node_left (node);
	}
	return 0;
}

Node *node_backrecurse (Node *node)
{
	if (node_up (node)) {
		node = node_up (node);
		while (node_right (node)) {
			node = node_right (node);
			node = node_bottom (node);
		}
		return (node);
	}
	return (node_left (node));
}

int node_no (Node *node)
{
	int no = 0;

	while (node) {
		node = node_backrecurse (node);
		no++;
	}
	return no;
}

Node *node_top (Node *node)
{
	if (node == 0)
		return 0;

	while (node_up (node))
		node = node_up (node);
	return (node);
}

Node *node_bottom (Node *node)
{
	if (node == 0)
		return 0;
	while (node_down (node))
		node = node_down (node);
	return node;
}

Node *node_insert_up (Node *node)
{
	Node *temp, *new = node_new ();

	temp = node->up;
	new->up = temp;
	new->down = node;

	node->up = new;
	if (temp)
		temp->down = new;

	new->left = node->left;

	if (node_left (new)) {		/* make tree consistent */
		if (node_right (node_left (node)) == node) {
			temp = node_left (new);
			temp->right = new;
		}
	}

	return new;
}

Node *node_insert_down (Node *node)
{
	Node *temp, *new = node_new ();

	temp = node->down;
	new->down = temp;
	new->up = node;

	node->down = new;
	if (temp)
		temp->up = new;

	new->left = node->left;

	return new;
}

Node *node_insert_right (Node *node)
{
	Node *new = node_new ();

	if ((!node) || (node->right)) {
		free (new);
		return 0;
	}

	new->left = node;
	node->right = new;

	return new;
}

unsigned int nodes_left (Node *node)
{
	unsigned int level = 0;

    while ((node = node_left (node))) {
		level++;
    }
    
	return level;
}

unsigned int nodes_up (Node *node)
{
	unsigned int level = 0;

	while ((node = node_up (node)))
		level++;
	return level;
}

unsigned int nodes_down (Node *node)
{
	unsigned int level = 0;

	while ((node = node_down (node)))
		level++;
	return level;
}

unsigned int nodes_right (Node *node)
{
	unsigned int level = 0;

	while ((node = node_right (node)))
		level++;
	return (level);
}

Node *node_remove (Node *node)
{
	Node *tup = node->up, *tdown = node->down;

	/* if we're wiping the tree, add a temp node for later reference to the empty tree */
	if ((node_left (node) == 0) && (node_up (node) == 0)
		&& (node_down (node) == 0)) {
		Node *tnode = node_insert_down (node);

		node_setflag (tnode, F_temp, 1);
		tdown = node_down (node);
	}

	/* remove all children */
	while (node_right (node))
		node_remove (node_right (node));

	/* close the gap in the linked list */
	if (tup)
		tup->down = tdown;
	if (tdown)
		tdown->up = tup;

	/* if we are a top-most child (parent says we are master of our siblings) */
	if ((node_left (node)) && (node_right (node_left (node)) == node)) {
		if (tdown)				/* rearrange parents pointer */
			node->left->right = tdown;
		else {					/* if no siblings remove ourselves, and return parent */
			Node *tnode = node_left (node);

			node->left->right = 0;
			node_free (node);
			return tnode;
		}
	}

	node_free (node);

	if (tup)
		return tup;
	if (tdown)
		return tdown;
	printf ("we're not where we should be\n");
	return 0;
}

Node *node_match (char *match, Node *where)
{
	Node *node;

	node = node_top (where);	/* do I want a match from top, or from where? */
	if (!match[0])
		return 0;

	do {
		if (strncmp
			(fixnullstring (node_get (node, TEXT)), match,
			 strlen (match)) == 0)
			return node;
	} while ((node = node_down (node)));

	return 0;
}

Node *node_exact_match (char *match, Node *where)
{
	Node *node;

	node = node_top (where);	/* see node_match */
	if (!match[0])
		return 0;

	do {
		if (strcmp (fixnullstring (node_get (node, TEXT)), match) == 0)
			return node;
	} while ((node = node_down (node)));

	return 0;
}

/* this is a commodity funciton, and I didn't want to code it myself,.. I
searched the fine web, found, cut'd, 'n', pasted.. 
 url: http://www.brokersys.com/snippets/STRISTR.C
*/

/*
** Designation:  StriStr
**
** Call syntax:  char *stristr(char *String, char *Pattern)
**
** Description:  This function is an ANSI version of strstr() with
**               case insensitivity.
**
** Return item:  char *pointer if Pattern is found in String, else
**               pointer to 0
**
** Rev History:  07/04/95  Bob Stout  ANSI-fy
**               02/03/94  Fred Cole  Original
**
** Hereby donated to public domain.
*/

static char *stristr (const char *String, const char *Pattern)
{
	char *pptr, *sptr, *start;
	long slen, plen;

	for (start = (char *) String, pptr = (char *) Pattern, slen = strlen (String), plen = strlen (Pattern);	/* while string length not shorter than pattern length */
		 slen >= plen; start++, slen--) {
		/* find start of pattern in string */
		while (toupper (*start) != toupper (*Pattern)) {
			start++;
			slen--;				/* if pattern longer than string */
			if (slen < plen)
				return (NULL);
		}
		sptr = start;
		pptr = (char *) Pattern;
		while (toupper (*sptr) == toupper (*pptr)) {
			sptr++;
			pptr++;				/* if end of pattern then pattern was found */
			if ('\0' == *pptr)
				return (start);
		}
	}
	return (NULL);
}

/*returns the next recursive node having match as a substring, or NULL if not found
  starting from where.
*/

Node *node_recursive_match (char *match, Node *where)
{
	if (!match[0])
		return NULL;

	where = node_recurse (where);	/* skip forward */
	while (where) {
		if (stristr (fixnullstring (node_get (where, TEXT)), match) != NULL)	/* case insensitive */
			return where;
		where = node_recurse (where);
	}

	return NULL;
}

Node *node_backrecursive_match (char *match, Node *where)
{
	if (!match[0])
		return NULL;

	where = node_backrecurse (where);	/* skip forward */
	while (where) {
		if (stristr (fixnullstring (node_get (where, TEXT)), match) != NULL)	/* case insensitive */
			return where;
		where = node_backrecurse (where);
	}

	return NULL;
}



Node *tree_new ()
{
	Node *root;

	root = node_new ();
	node_setflags (root, F_temp);
	return root;
}

Node *node_root (Node *node)
{
	while (node_left (node))
		node = node_left (node);
	node = node_top (node);
	return node;
}

void tree_free (Node *node)
{
	Node *root = node_root (node);

	while (node_down (root))
		node_remove (node_down (root));

	root = node_remove (root);
	node_free (root);

	return;
}

/*
	swaps the positions in the tree of the two specified nodes
*/
void node_swap (Node *nodeA, Node *nodeB)
{
	Node *Aup, *Aleft, *Aright, *Adown;
	Node *Bup, *Bleft, *Bright, *Bdown;

	if ((!nodeB) || (!nodeA))
		return;

	if (nodeB == nodeA)
		return;

	if (nodeB->right == nodeA || nodeA->right == nodeB) {
		return;					/* can't swap parent and child,.. (nor deeper levels actually) */
	}

	if ((nodeB->down == nodeA) && (nodeA->up == nodeB)) {	/* special case neighbours,.. normalize first */
		Node *tnode = nodeA;

		nodeA = nodeB;
		nodeB = tnode;
	}

	Aup = node_up (nodeA);
	Adown = node_down (nodeA);
	Aleft = node_left (nodeA);
	Aright = node_right (nodeA);
	Bup = node_up (nodeB);
	Bdown = node_down (nodeB);
	Bleft = node_left (nodeB);
	Bright = node_right (nodeB);

	if ((nodeA->down == nodeB) && (nodeB->up == nodeA)) {	/* special case, neighbours */
		if (Aup)
			Aup->down = nodeB;
		nodeB->up = Aup;
		if (Bdown)
			Bdown->up = nodeA;
		nodeA->down = Bdown;
		nodeA->up = nodeB;
		nodeB->down = nodeA;
		if (Aleft)
			if (Aleft->right == nodeA)
				Aleft->right = nodeB;
		return;
	}

	if (Aup)
		Aup->down = nodeB;
	nodeB->up = Aup;
	if (Adown)
		Adown->up = nodeB;
	nodeB->down = Adown;
	if (Aleft) {
		if (Aleft->right == nodeA)
			Aleft->right = nodeB;

	}
	nodeB->left = Aleft;

	if (Bup)
		Bup->down = nodeA;
	nodeA->up = Bup;
	if (Bdown)
		Bdown->up = nodeA;
	nodeA->down = Bdown;

	if (Bleft) {
		if (Bleft->right == nodeB)
			Bleft->right = nodeA;
	}
	nodeA->left = Bleft;
}


#include "file.h"

Node *tree_duplicate (Node *source, Node *target)
{
	int level, startlevel;
	import_state_t ist;
	Node *tnode;

	tnode = node_duplicate (source);
	tnode->up = tnode->down = tnode->left = tnode->right = NULL;
	node_swap (tnode, target);
	node_free (target);
	target = tnode;

	init_import (&ist, target);

	if (node_right (source)) {
		source = node_right (source);
		startlevel = nodes_left (source);
		while ((source) && (nodes_left (source) >= startlevel)) {
			Node *tnode;

			level = nodes_left (source) - startlevel + 1;
			tnode = node_duplicate (source);

			/* clear out all references to other nodes */
			tnode->up = tnode->down = tnode->left = tnode->right = NULL;
			import_node (&ist, level, tnode);
			source = node_recurse (source);
		}
	}

	return target;
}
