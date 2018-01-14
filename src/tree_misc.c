/*
 * tree_misc.c -- various functions bindable/callable from hnb, should be seperated
 *                out into seperate files
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

#include <string.h>
#include "tree.h"
#include "node.h"
#include "prefs.h"
#include "ui.h"
#include "ui_cli.h"
#include "evilloop.h"

static void* cmd_movenode (int argc, char **argv, void *data)
{
	Node *pos = (Node *) data;
	if(argc<2){
		cli_outfunf("usage: %s <left|right|up|down>",argv[0]);
		return pos;
	}
	if(prefs.readonly){
		cli_outfun("readonly flag set, avoiding tree change");
		return data;
	}
	if (!strcmp (argv[1], "left")) {
		if (node_left (pos)) {
			Node *tnode;

			tnode = node_insert_down (node_left (pos));
			node_swap (pos, tnode);
			node_remove (tnode);
		}
	} else if (!strcmp (argv[1], "right")) {
		if (node_up (pos)) {
			Node *tnode;

			tnode = node_up (pos);
			if (!node_right (tnode)) {
				tnode = node_insert_right (tnode);
			} else {
				tnode = node_insert_down (node_bottom (node_right (tnode)));
			}
			node_swap (pos, tnode);
			node_remove (tnode);
		}
	} else if (!strcmp (argv[1], "up")) {
		if (node_up (pos)) {
			node_swap (pos, node_up (pos));
		}
	} else if (!strcmp (argv[1], "down")) {
		if (node_down (pos)) {
			node_swap (pos, node_down (pos));
		}
	}
	docmd(pos,"tree_changed");
	return pos;
}

/*
!init_movenode();
*/
void init_movenode ()
{
	cli_add_command ("movenode", cmd_movenode, "<up|left|right|down>");
	cli_add_help ("movenode", "Moves the node in the specified direction.");
}

static void* cmd_go(int argc, char **argv, void *data){
	Node *pos=(Node *)data;
	
	if(argc!=2){
		cli_outfunf("usage: %s <up|down|left|right|recurse[-visible]"
				"|backrecurse[-visible]|root|top|bottom>");
		return pos;
	}
	
	if(!strcmp(argv[1],"up")){
		if(node_up(pos))
			pos=node_up(pos);
	} else if(!strcmp(argv[1],"down")){
		if(node_down(pos))
			pos=node_down(pos);
	} else if(!strcmp(argv[1],"left")){
		if(node_left(pos))
			pos=node_left(pos);
	} else if(!strcmp(argv[1],"right")){
		if(node_right(pos))
			pos=node_right(pos);
	} else if(!strcmp(argv[1],"recurse")){
		if(node_recurse(pos))
			pos=node_recurse(pos);
	} else if(!strcmp(argv[1],"backrecurse")){
		if(node_backrecurse(pos))
			pos=node_backrecurse(pos);
	} else if(!strcmp(argv[1],"recurse-visible")){
		if(node_recurse_visible(pos))
			pos=node_recurse_visible(pos);
	} else if(!strcmp(argv[1],"backrecurse-visible")){
		if(node_backrecurse_visible(pos))
			pos=node_backrecurse_visible(pos);
	} else if(!strcmp(argv[1],"root")){
		pos=node_root(pos);
	} else if(!strcmp(argv[1],"top")){
		pos=node_top(pos);
	} else if(!strcmp(argv[1],"bottom")){
		pos=node_bottom(pos);
	}

	
	return pos;
}

/*
!init_go();
*/
void init_go ()
{
	cli_add_command ("go", cmd_go, "<up|down|left|right|recurse|backrecurse|root|top|bottom>");
	cli_add_help ("go", "Changes the current position in the tree.");
}

#include "evilloop.h"
#include "ctype.h"
#include "ui_binding.h"

static void* cmd_outdent (int argc, char **argv, void *data)
{
	Node *pos = (Node *) data;

	if(inputbuf[0] && isprint(lastbinding->key)){ /* evil workaround */
		if (lastbinding->key > 31 && lastbinding->key < 255) {	/*  input for buffer */
			inputbuf[strlen (inputbuf) + 1] = 0;
			inputbuf[strlen (inputbuf)] = lastbinding->key;
		}		
		return pos;
	}
	if(prefs.readonly){
		cli_outfun("readonly flag set, avoiding tree change");
		return data;
	}

	if (node_left (pos)) {
		Node *target_node = node_left (pos);
		Node *last_node = node_bottom (pos);
		Node *first_node = pos;
		Node *prev_up = node_up (pos);
		Node *prev_target_down = node_down (target_node);
		Node *tnode;

		tnode = first_node;
		while (tnode != last_node) {
			tnode->left = target_node->left;
			tnode = tnode->down;
		};
		tnode->left = target_node->left;

		first_node->up = target_node;
		target_node->down = first_node;
		last_node->down = prev_target_down;

		if (prev_target_down)
			prev_target_down->up = last_node;

		if (prev_up) {
			prev_up->down = NULL;
		} else {
			target_node->right = NULL;
		}
		docmd(pos,"tree_changed");
	}
	return pos;
}

/*	FIXME: no real need for a temporary node */

static void* cmd_indent (int argc, char **argv, void *data)
{
	Node *pos = (Node *) data;

	if(inputbuf[0] && isprint(lastbinding->key)){ /* evil workaround */
		if (lastbinding->key > 31 && lastbinding->key < 255) {	/*  input for buffer */
			inputbuf[strlen (inputbuf) + 1] = 0;
			inputbuf[strlen (inputbuf)] = lastbinding->key;
		}		
		return pos;
	}
	if(prefs.readonly){
		cli_outfun("readonly flag set, avoiding tree change");
		return data;
	}

	if (node_up (pos)) {
		Node *tnode, *snode;

		tnode = node_up (pos);
		if (!node_right (tnode)) {
			tnode = node_insert_right (tnode);
		} else {
			tnode = node_insert_down (node_bottom (node_right (tnode)));
		}
		node_swap (pos, tnode);
		snode = tnode;

		node_up (snode)->down = NULL;
		snode->up = pos;
		pos->down = snode;

		while (snode) {
			snode->left = pos->left;
			snode = node_down (snode);
		}
		node_remove (node_down (pos));
		docmd(pos,"tree_changed");
	}
	return pos;
}

/*
!init_outdent_indent();
*/
void init_outdent_indent ()
{
	cli_add_command ("outdent", cmd_outdent, "");
	cli_add_help ("outdent",
		"Moves the current entry and any following siblings one level to the left.");
	cli_add_command ("indent", cmd_indent, "");
	cli_add_help ("indent",
		"Moves the current entry and any following siblings one level to the right.");
}

static void* remove_cmd (int argc, char **argv, void *data)
{
	Node *pos = (Node *) data;

	if(prefs.readonly){
		cli_outfun("readonly flag set, avoiding tree change");
		return data;
	}
	if (node_right (pos)) {
		Tbinding *c;
		int tempscope = ui_current_scope;

		ui_current_scope = ui_scope_confirm;
		docmdf (pos, "status 'node has children, really remove?'");
		ui_draw (pos, "", 0);
		ui_current_scope = tempscope;
		c = parsekey (ui_input (), ui_scope_confirm);
		if (c->action == ui_action_confirm) {
			docmd (pos, "save_state");
			pos = node_remove (pos);
			docmd(pos,"tree_changed");
		}
	} else {
		docmd (pos, "save_state");
		pos = node_remove (pos);
		docmd(pos,"tree_changed");
	}
	return pos;
}

/*
!init_remove();
*/
void init_remove ()
{
	cli_add_command ("remove", remove_cmd, "");
	cli_add_help ("remove",
		"Removes the active node. If it has children, Tines confirms you want to delete it.");
}


static void* commandline_cmd (int argc, char **argv, void *data)
{
	Node *pos = (Node *) data;

	char commandline[80];

	do {
		strcpy (commandline, "");
		ui_draw (pos, "", 0);
		ui_getstr ("Commandline interface, enter blank command to exit",
				   commandline);

		if (commandline[0])
			pos = docmd (pos, commandline);
	} while (commandline[0] && strcmp(commandline,"q") && strcmp(commandline,"quit"));
	return pos;
}

/*
!init_commandline();
*/
void init_commandline ()
{
	cli_add_command ("commandline", commandline_cmd, "");
	cli_add_help ("commandline",
		"Starts an interactive commandline from curses mode.");
}

static void* insert_below_cmd (int argc, char **argv, void *data)
{
	Node *pos = (Node *) data;
	if(prefs.readonly){
		cli_outfun("readonly flag set, avoiding tree change");
		return data;
	}

	if (node_getflag (pos, F_temp)) {
		node_toggleflag (pos, F_temp);
	} else {
		pos = node_insert_down (pos);
		if (node_left (pos))
			if (!strcmp(fixnullstring(node_get(node_left(pos),"type")),"todo")){
				node_set (pos, "type", "todo");
				node_set (pos, "done", "no");
			}
			if (!strcmp(fixnullstring(node_get(node_left(pos),"type")),"text")){
				node_set (pos, "type", "text");
			}
	}
	inputbuf[0] = 0;
	docmd(pos,"tree_changed");
	return pos;
}

/*
!init_insertbelow();
*/
void init_insertbelow ()
{
	cli_add_command ("insert_below", insert_below_cmd, "");
	cli_add_help ("insert_below",
		"Adds a new entry immediatly below the current entry. The new entry has the same attributes as the current entry.");
}

static void* widen_narrow_cmd (int argc, char **argv, void *data)
{
	Node *pos = (Node *) data;
	if (!global_tree_narrow.is_narrowed) {
		pos = tree_narrow (pos, &global_tree_narrow);
	} else {
		pos = tree_widen (pos, &global_tree_narrow);
	}
	return pos;
}

static void* widen_narrow_region_cmd (int argc, char **argv, void *data)
{
	Node *pos = (Node *) data;
	Node *sp = pos;
	if (node_left (pos)) {
		pos = node_left (pos);
		docmd(pos, "narrow_or_widen");
		pos = sp;
	}
	return pos;
}

/*
!init_widen_narrow();
*/
void init_widen_narrow ()
{
	cli_add_command ("narrow_or_widen", widen_narrow_cmd, "");
	cli_add_help ("narrow_or_widen", "Narrows the view to the currently selected node and its children, temporarily severing the rest of the tree. EXPERIMENTAL. Use with care. Don't save while the view is narrow or you WILL lose data.");
	cli_add_command ("narrow_or_widen_region", widen_narrow_region_cmd, "");
	cli_add_help ("narrow_or_widen_region", "Narrows the view to the region your cursor is in -- that is, the current node, nodes around it at the same level, and its parent. EXPERIMENTAL. Use with care. Don't save while the view is narrow or you WILL lose data.");
}

/*
	TODO:
		setting of attributes,.. percentage, size, donebydate etc.
*/
