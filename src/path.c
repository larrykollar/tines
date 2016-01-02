/*
 * path.c -- converts paths into node pointers and vice versa
 *
 * Copyright (C) 2001,2001 Øyvind Kolås <pippin@users.sourceforge.net>
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
#include <stdio.h>
#include "tree.h"
#include "tokenizer.h"
#include "path.h"

/*
	converts a best matching node to the path.. (path tokens start
	as specified)
*/
Node *matchpath2node (char *path, Node *start)
{
	char *token;
	Node *node;					/* should perhaps be a relative start node? */

	node = start;

	token_seperator = '/';

	token = tokenize (path);
	if (!strcmp (token, "/")) {
		node = node_root (start);
		token = tokenize ("");
		if (token[0] != '/')	/* this is returned if there was only a "/" in path dunno why */
			node = node_match (token, node);
		if (!node)
			return 0;
	} else {
		node = node_match (token, node);
		if (!node)
			return 0;
	}
	token = tokenize ("");

	while (token[0] != 0) {
		if (node_right (node))
			node = node_right (node);
		if (strcmp (token, "/"))	/* it is not the lonely / at the end */
			node = node_match (token, node);
		if (!node)
			return 0;
		token = tokenize ("");
	}

	return (node);
}

/**
 converts a pathstring to a node in the
 tree, ps, the tokenizer escapes
 double //'s as a single / without
 splitting the things at the sides
 a /// means,.. a slash at the end
 of token, and new token coming
*/

Node *path2node (char *path, Node *start)
{
	char *token;
	Node *node;					/* should perhaps be a relative start node? */

	node = start;

	token_seperator = '/';

	token = tokenize (path);
	if (!strcmp (token, "/")) {
		node = node_root (start);
		token = tokenize ("");
		if (token[0] != '/')	/* this is returned if there was only a "/" in path dunno why */
			node = node_exact_match (token, node);
		if (!node)
			return 0;
	} else {
		node = node_exact_match (token, node);
		if (!node)
			return 0;
	}
	token = tokenize ("");

	while (token[0] != 0) {
		if (node_right (node))
			node = node_right (node);
		if (strcmp (token, "/"))	/* it is not the lonely / at the end */
			node = node_exact_match (token, node);
		if (!node)
			return 0;
		token = tokenize ("");
	}

	return (node);
}

/* same as above,.. but forcing and recursive  (creates the nodes as we go if they don't exist) */

Node *path2node_make (char *path, Node *root)
{
	char *token;
	Node *node;					/* should perhaps be a relative start node? */

	node = root;

	token_seperator = '/';

	token = tokenize (path);
	if (!strcmp (token, "/")) {
		node = root;
		token = tokenize ("");
		if (token[0] != 0) {
			Node *tnode;

			tnode = node_exact_match (token, node);	/* something is amiss here? */
			if (tnode == 0) {
				tnode = node_insert_down (node);
				node_set (tnode, TEXT, token);
			}
			node = tnode;
		}
	} else {
		node = node_exact_match (token, node);
	}
	token = tokenize ("");

	while (token[0] != 0) {
		if (node_right (node)) {
			node = node_right (node);
			if (strcmp (token, "/")) {	/* it is not the lonely / at the end */
				Node *tnode;

				tnode = node_exact_match (token, node);
				if (tnode == 0) {
					tnode = node_insert_down (node);
					node_set (tnode, TEXT, token);
				}
				node = tnode;
			}
		} else {				/* we must create a child */
			node = node_insert_right (node);
			if (strcmp (token, "/")) {	/* it is not the lonely / at the end */
				node_set (node, TEXT, token);
			}
		}
		token = tokenize ("");
	}

	/* if there is a node below,.. we should remove it,...   we don't want duplicate entries.. */

	if (node_up (node))
		node = node_remove (node_up (node));
	return (node);
}

/*	creates an absolute path like 
	/aaa/bbb/ccc
	for the node (ccc) specified 
	
	returns the path from a static array
*/
char *node2path (Node *node)
{
	static char path[2048];
	long pos = 0;
	int levels = nodes_left (node);
	int cnt;

	path[pos] = '/';
	path[++pos] = 0;

	for (cnt = levels; cnt >= 0; cnt--) {
		int cnt2;
		Node *tnode = node;

		for (cnt2 = 0; cnt2 < cnt; cnt2++)
			tnode = node_left (tnode);

		strcpy (&path[pos], fixnullstring (node_get (tnode, TEXT)));
		pos = strlen (path);
		path[pos] = '/';
		path[++pos] = 0;
	}

	path[--pos] = 0;

	return (path);
}

char *node2no_path (Node *node)
{
	static char path[512];
	long pos = 0;
	int levels = nodes_left (node);
	int cnt;

	path[pos] = '/';
	path[++pos] = 0;

	for (cnt = levels; cnt >= 0; cnt--) {
		int cnt2;
		Node *tnode = node;

		for (cnt2 = 0; cnt2 < cnt; cnt2++)
			tnode = node_left (tnode);

		/*strcpy(&path[pos], tnode->data); */
		sprintf (&path[pos], "%i", nodes_up (tnode));
		pos = strlen (path);
		path[pos] = '/';
		path[++pos] = 0;
	}

	path[--pos] = 0;

	return (path);
}

/*	/0/3/2/
	means:
		/ goto root
		0 go down none
		/ go right
		3 go down three
		/ go right
		2 go down two
		/ go right*/

Node *no_path2node (char *path, Node *root)
{
	char *token;
	Node *node;					/* should perhaps be a relative start node? */

	node = root;

	token_seperator = '/';

	token = tokenize (path);
	if (!strcmp (token, "/")) {
		node = root;
		token = tokenize ("");
		if (token[0] != 0) {
			int no = atoi (token);

			for (; no > 0; no--)
				node = node_down (node);
		}

	} else {
		int no = atoi (token);

		for (; no > 0; no--)
			node = node_down (node);
	}
	token = tokenize ("");

	while (token[0] != 0) {
		if (node_right (node))
			node = node_right (node);
		if (strcmp (token, "/")) {	/* it is not the lonely / at the end */
			int no = atoi (token);

			for (; no > 0; no--)
				node = node_down (node);
		}
		token = tokenize ("");
	}

	return (node);
}


typedef struct Path {
	struct Path *parent;
	struct Node *node;
} Path;
