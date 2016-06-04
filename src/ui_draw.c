/*
 * ui_draw.c -- drawing of a nodes and trees using curses
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
#include <assert.h>
#include "tree.h"
#include "tree_todo.h"
#include <string.h>
#include <unistd.h>
#include <curses.h>
#include "stdio.h"
#include "prefs.h"
#include "ui_overlay.h"
#define UI_C
#include "ui.h"
#include "ui_draw.h"
#include "cli.h"
#include <stdlib.h>
#include <ctype.h>

#define KEEPLINES 5

extern int nodes_above;
extern int active_line;
extern int nodes_below;

static Node *up (Node *sel, Node *node)
{
	if (node_up (node) && node_getflag( node_up (node), F_expanded)) {
		node = node_up (node);
		while (node_right (node) && node_getflag(node,F_expanded)) {
			node = node_right (node);
			node = node_bottom (node);
		}
		return (node);
	} else {
		if (node_up (node))
			return (node_up (node));
		else
			return (node_left (node));
	}
	return node_left (node);
}

static Node *down (Node *sel, Node *node)
{
	if (node_getflag(node,F_expanded)) {
		return node_recurse (node);
	} else {
		if (node_down (node)) {
			return (node_down (node));
		} else {
			while (node != 0) {
				node = node_left (node);
				if (node_down (node))
					return (node_down (node));
			}
		}
	}
	return NULL;
}

int startlevel = 0;

long hnb_edit_posup = 0;			/*contains the cursor pos for up/down */
long hnb_edit_posdown = 0;		/*from here when in editing mode */

enum {
	drawmode_test = 0,
	drawmode_normal,
	drawmode_selected,
	drawmode_edit,
	drawmode_completion
};

/* draws the actual node data with word wrapping, should be reengineered into a general
 * linewrapping function.
 *
 *
 */
static int draw_textblock (int line_start, int col_start, int width,
						   int cursor_pos, Node *node, int drawmode)
{
	int col_end = col_start + width;
	unsigned char word[200];	/* current word being rendered */
	int wpos = 0;				/* position in current word */
	int dpos = 0;				/* position in data */
	int col;					/* current column */

	int lines_used = 1;

	int cursor_state = 0;
	int cx = 0, cy = 0;			/* coordinates to draw cursor at */

	unsigned char *data =
		(unsigned char *) fixnullstring (node_get (node, TEXT));

	/* when line is empty, put a space to show up on some terminals */
	if (data[0] == '\0') {
	  data = (unsigned char *)" ";
	}

	/* when line is empty, put a space to show up on some terminals */
	if (data[0] == '\0') {
	  data = (unsigned char *)" ";
	}

	col = col_start;

	word[0] = 0;
	if (drawmode == drawmode_edit) {
		hnb_edit_posup = 0;
		hnb_edit_posdown = strlen ((char *) data);
	}

	switch (drawmode) {
		case drawmode_test:
			break;
		case drawmode_completion:
			if (node_right (node)) {
				ui_style (ui_style_parentnode);
			} else {
				ui_style (ui_style_node);
			}
			break;
		case drawmode_selected:
			if (node_right (node)) {
				ui_style (ui_style_parentselected);
			} else {
				ui_style (ui_style_selected);
			}
			break;
		case drawmode_normal:
		case drawmode_edit:
		default:
			if (node_right (node)) {
				ui_style (ui_style_parentnode);
			} else {
				ui_style (ui_style_node);
			}
			break;
	}

	while ((dpos == 0) || data[dpos - 1]) {	/* loop through data + \0 */
		switch (data[dpos]) {
			case '\0':			/* \0 as well,.. to print last word */
			case ' ':
			case '\t':
			case '\n':
			case '\r':			/* all whitespace is treated as spaces */
			wordwrap:
				if (col + wpos + 1 >= col_end) {	/* reached margin */
					if (drawmode == drawmode_edit) {
						if (cursor_state == 0)
							hnb_edit_posup = cursor_pos - (col - col_start);
						if (cursor_state == 1) {
							hnb_edit_posdown = cursor_pos + (col - col_start);
							cursor_state = 2;
						}
					}

					col = col_start;
					lines_used++;
					if (lines_used + line_start >= LINES)
						return lines_used;
				}
				if (drawmode != drawmode_test) {
					if (line_start + lines_used - 1 >= 0) {
						move (line_start + lines_used - 1, col);

						/* must break the word in two due to differnt text formatting */
						if (drawmode == drawmode_completion
							&& cursor_state == 0 && dpos >= cursor_pos) {
							int i;

							for (i = 0; i < wpos - (dpos - cursor_pos); i++)
								addch (word[i]);
							if (node_right (node)) {
								ui_style (ui_style_parentselected);
							} else {
								ui_style (ui_style_selected);
							}
							for (i = wpos - (dpos - cursor_pos); i < wpos;
								 i++)
								addch (word[i]);
						} else {

							addstr ((char *) word);

						}
						if (data[dpos]) {
							if (isspace(data[dpos]))
								addch (' ');
							else
								addch (data[dpos]);
						}
					}
				}

				switch (drawmode) {
					case drawmode_edit:
						if (cursor_state == 0 && dpos >= cursor_pos) {
							cy = line_start + lines_used - 1;
							cx = col - (dpos - cursor_pos) + wpos;
							cursor_state = 1;
						}
						break;
					case drawmode_completion:
						if (cursor_state == 0 && dpos >= cursor_pos) {
							if (node_right (node)) {
								ui_style (ui_style_parentselected);
							} else {
								ui_style (ui_style_selected);
							}
							cursor_state = 1;
						}
					default:
						break;
				}

				col += wpos + 1;
				word[wpos = 0] = 0;
				break;
			default:
				if (col_start + wpos + 2 >= col_end)
					goto wordwrap;

				word[wpos++] = data[dpos];
				word[wpos] = 0;

				break;
		}
		dpos++;
	}
	/* draw the cursor */
	if (drawmode == drawmode_edit) {
		move (cy, cx);
		if (node_right (node)) {
			ui_style (ui_style_parentselected);
		} else {
			ui_style (ui_style_selected);
		}
		addch(A_CHARTEXT & inch());
		if (node_right (node)) {
			ui_style (ui_style_parentnode);
		} else {
			ui_style (ui_style_node);
		}
	}
	return lines_used;
}



static int draw_dummy (int line, int col, int width, Node *node, int drawmode)
{
	if (width == 0)
		width = 1;
	if (drawmode != drawmode_test) {
		int j;

		move (line, col);
		ui_style (ui_style_bullet);
		for (j = 0; j < width; j++) {
			addch ('X');
		}
	}
	return width;
}

static int draw_spacing (int line, int col, int width, Node *node,
						 int drawmode)
{
	if (width == 0)
		width = 1;
	if (drawmode != drawmode_test) {
		int j;

		move (line, col);
		ui_style (ui_style_background);
		for (j = 0; j < width; j++) {
			addch (' ');
		}
	}
	return width;
}

/* 13: three four-byte characters + null */
static char bullet_leaf[13] = "  *";
static char bullet_parent[13] = "  +";
static char bullet_parent_expanded[13] = "  -";
static char bullet_todo_leaf_undone[13] = "[ ]";
static char bullet_todo_leaf_done[13] = "[X]";
static char bullet_todo_parent_undone[13] = "[ ]";
static char bullet_todo_parent_done[13] = "[X]";
static char bullet_textleaf[13] = "t *";
static char bullet_textparent[13] = "t +";
static char bullet_textparent_expanded[13] = "t -";


static int draw_bullet (int line, int col, int width, Node *node,
						int drawmode)
{
	int asize;
	int perc;

/*	if(width==0)*/
	width = 3;

	perc = calc_percentage_size (node, &asize);
	{
		ui_style (ui_style_bullet);

		move (line, col);
		switch (perc) {
			/* no todo info */
			case -1:
				if (strcmp(fixnullstring(node_get(node, "type")), "text")) {
					/* not a text node */
					if (drawmode != drawmode_test) {
						addstr ((node_right (node)) ? node_getflag(node,F_expanded)
								? bullet_parent_expanded : bullet_parent
								: bullet_leaf);
					}
				} else {
					if (drawmode != drawmode_test) {
						addstr ((node_right (node)) ? node_getflag(node,F_expanded)
								? bullet_textparent_expanded : bullet_textparent
								: bullet_textleaf);
					}
				}
				break;
			/* not started */
			case 0:
				if (drawmode != drawmode_test)
					addstr ((node_right (node)) ? bullet_todo_parent_undone : bullet_todo_leaf_undone);
				break;
			/* done */
			case 2000:
				if (drawmode != drawmode_test)
					addstr ((node_right (node)) ? bullet_todo_parent_done : bullet_todo_leaf_done);
				break;
			/* value = 1..1000 (1..100%) */
			default:{
				char str[100];

				snprintf (str, 4, "%2i%%", perc);
				if (drawmode != drawmode_test)
					addstr (str);
			}
		}
	}

	return width;
}


static char *node2no_path (Node *node)
{
	static char path[512];
	long pos = 0;
	int levels = nodes_left (node);
	int cnt;

	path[0] = 0;

	for (cnt = levels; cnt >= 0; cnt--) {
		int cnt2;
		Node *tnode = node;

		for (cnt2 = 0; cnt2 < cnt; cnt2++)
			tnode = node_left (tnode);

		sprintf (&path[pos], "%i", nodes_up (tnode) + 1);
		pos = strlen (path);
		path[pos] = '.';
		path[++pos] = 0;
	}

	path[--pos] = 0;

	return (path);
}

static int draw_node_no (int line, int col, int width, Node *node,
						 int drawmode)
{
	char str[100] = "";

	if (width == 0)
		width = 4;

	ui_style (ui_style_bullet);
	move (line, col);
	snprintf (str, 5, "%4i", node_no (node));
	if (drawmode != drawmode_test) {
		addstr (str);
	}

	return width;
}


static int draw_nr (int line, int col, int width, Node *node, int drawmode)
{
	char str[100] = "";

	if (width == 0)
		width = 3;

	ui_style (ui_style_bullet);
	move (line, col);
	snprintf (str, 5, "%3i", nodes_up (node) + 1);
	if (drawmode != drawmode_test) {
		addstr (str);
	}

	return width;
}



static int draw_anr (int line, int col, int width, Node *node, int drawmode)
{
	char str[100] = "";
	char fstr[20];

	if (width == 0)
		width = 8;

	ui_style (ui_style_bullet);
	move (line, col);
	snprintf (fstr, 8, "%%%is", width);
	snprintf (str, width + 2, fstr, node2no_path (node));
	if (drawmode != drawmode_test) {
		addstr (str);
	}

	return width;
}

static int node_getval(Node *node, char *name){
	char *got=node_get(node,name);
	if(!got)return -1;
	return(atoi(got));
}


static int draw_debug (int line, int col, int width, Node *node, int drawmode)
{
	int asize;
	int size = node_getval (node,"size");
	int perc;

	width = 40;

	if (drawmode != drawmode_test) {
		ui_style (ui_style_background);
		move (line, col);
		perc = calc_percentage_size (node, &asize);

		{
			char str[64];

			sprintf (str, "(%i/%i) ",
					 (perc == 2000 ? 100 : perc * asize) / 100, asize);
			if (drawmode != drawmode_test)
				addstr (str);
		}

		{
			perc = calc_percentage_size (node, &asize);
			attrset (A_NORMAL);
			{
				char str[256];

				sprintf (str, "size:%i a_size:%i %i%% ", node_getval (node,"size"),
						 size, perc);
				addstr (str);
			}
		}

		if (node_calc_size (node) != -1) {
			char str[10];

			sprintf (str, "%4.1f ", (float) node_calc_size (node) / 10.0);
			addstr (str);
		}
	}

	return width;
}

#define MAX_COLUMNS 20

static int draw_indent (int line, int col, int width, Node *node,
						int drawmode)
{
	if (width == 0)
		width = 4;

	return width * nodes_left (node);
}


enum {
	col_spacing = 0,
	col_indent,
	col_nr,
	col_anr,
	col_bullet,
	col_data,
	col_debug,
	col_percentage,
	col_node_no,
	col_dummy,
	col_terminate
};


static int (*col_fun[col_terminate + 1]) (int line, int col, int width,
										  Node *node, int drawmode) = {
draw_spacing, draw_indent, draw_nr, draw_anr, draw_bullet, draw_spacing,
		draw_debug, draw_spacing, draw_node_no, draw_dummy, draw_dummy};

static struct {
	int type;
	int width;
} col_def[MAX_COLUMNS] = {
	{
	col_indent, 4}, {
	col_spacing, 1}, {
	col_bullet, 3}, {
	col_spacing, 1}, {
	col_data, 0}, {
	col_spacing, 1}, {
	col_dummy, 10}, {
	col_spacing, 2}, {
	col_dummy, 10}, {
	col_spacing, 1}, {
	col_terminate, 0}
};


/* FIXME: make backup?,.. and make sure data is present,.., make possiblity to write back? */

void* display_format_cmd (int argc, char **argv, void *data)
{
	char *p = argv[1];
	int width;
	int type;
	int col_no = 0;

	if(argc<2){
		return data;
	}

	do {
		width = 0;
		type = col_spacing;
		switch (*p) {
			case 'i':
				type = col_indent;
				if (isdigit (*(p + 1))) {
					width = atoi (p + 1);
					while (isdigit ((unsigned char)*(p + 1)))
						p++;
				}
				break;
			case 'd':
				type = col_data;
				if (isdigit (*(p + 1))) {
					width = atoi (p + 1);
					while (isdigit ((unsigned char)*(p + 1)))
						p++;
				}
				break;
			case 'D':
				type = col_debug;
				if (isdigit (*(p + 1))) {
					width = atoi (p + 1);
					while (isdigit ((unsigned char)*(p + 1)))
						p++;
				}
				break;
			case 'x':
				type = col_dummy;
				if (isdigit (*(p + 1))) {
					width = atoi (p + 1);
					while (isdigit ((unsigned char)*(p + 1)))
						p++;
				}
				break;
			case '1':
				type = col_nr;
				if (*(p + 1) == '.') {
					type = col_anr;
					p++;
				}
				if (isdigit (*(p + 1))) {
					width = atoi (p + 1);
					while (isdigit ((unsigned char)*(p + 1)))
						p++;
				}
				break;
			case '-':
				type = col_bullet;
				if (isdigit (*(p + 1))) {
					width = atoi (p + 1);
					while (isdigit ((unsigned char)*(p + 1)))
						p++;
				}
				break;
			case '#':
				type = col_node_no;
				if (isdigit (*(p + 1))) {
					width = atoi (p + 1);
					while (isdigit ((unsigned char)*(p + 1)))
						p++;
				}
				break;
			case ' ':
				type = col_spacing;
				while (' ' == ((unsigned char)*(p + 1))) {
					p++;
					width++;
				}
				break;
			default:
				cli_outfunf ("td not_parsed(%c)", *p);
				break;
		}
		col_def[col_no].type = type;
		col_def[col_no].width = width;
		col_no++;
	} while (*(++p));

	col_def[col_no].type = col_terminate;

	return data;
}





/*
 * @param line_start which line on the display the first line of the draw node is on
 * @param level      the indentation level of this item
 * @param node       the node to draw
 * @param cursor_pos different meanings in different modes, testmode: none
 *                   highlightmode: none, edit_mode: the position in the data
 *                   that should be highlighted,
 *                   completion: the number of matched chars in data
 *                   
 * @param draw_mode  1=draw, 0=test
 *
 * @return number of lines needed to draw item
 **/
static int draw_item (int line_start, int cursor_pos, Node *node,
					  int drawmode)
{
	int col_no = 0;
	int lines_used = 1;

	int col_start = 0;
	int col_end = COLS;

	col_start = 0;
	/* draw columns before col_data */

	while (col_def[col_no].type != col_data
		   && col_def[col_no].type != col_terminate) {
		col_start +=
			col_fun[col_def[col_no].type] (line_start, col_start,
										   col_def[col_no].width, node,
										   drawmode);
		col_no++;
	}

	/* fastforward to end of col_def */
	while (col_def[col_no].type != col_terminate)
		col_no++;

	col_no--;

	/* draw columns after col_data */
	while (col_no && col_def[col_no].type != col_data) {
		int width =
			col_fun[col_def[col_no].type] (line_start,
										   col_end - col_def[col_no].width,
										   col_def[col_no].width, node,
										   drawmode_test);
		col_end -=
			col_fun[col_def[col_no].type] (line_start, col_end - width, width,
										   node, drawmode);
		col_no--;
	}

	lines_used =
		draw_textblock (line_start, col_start, (col_end - col_start),
						cursor_pos, node, drawmode);

	return lines_used;
}


#define MAXLINES 512
static long line_nodeno[MAXLINES] = { 0 };

void ui_draw (Node *node, char *input, int edit_mode)
{
	int lines;

	int hnb_nodes_up;
	int hnb_nodes_down;

	static struct {
		long self;
		long prev;
	} node_numb = {
	1, 1};

	if (!prefs.fixedfocus) {

		node_numb.prev = node_numb.self;
		node_numb.self = node_no (node);

		if (node_numb.self > node_numb.prev) {
			active_line++;
		} else if (node_numb.self < node_numb.prev) {
			active_line--;
		}

		{
			int i;

			for (i = 0; i < ((LINES < MAXLINES) ? LINES : MAXLINES); i++)
				if (line_nodeno[i] == node_numb.self) {
					active_line = i;
					break;
				}
		}

		if (node_numb.self == 1) {	/* jumped to root, always bring nodes to top of screen */
			active_line = 1;
		}

		{
			int i;

			for (i = 0; i < ((LINES < MAXLINES) ? LINES : MAXLINES); i++)
				line_nodeno[i] = 0;
		}


		{
			int maxline = LINES - KEEPLINES;

			if (active_line > maxline)	/*if we overlap with help,.. move up */
				active_line = maxline;
			if (active_line < KEEPLINES)
				active_line = KEEPLINES;
		}
	};

	nodes_above = active_line;
	nodes_below = LINES - active_line;

	{
		hnb_nodes_up = 0;
		hnb_nodes_down = 0;

		erase ();
/* draw nodes above selected node */
		{
			Node *prev_down = node;	/* to aid pgup/pgdn */
			int line = active_line;
			Node *tnode = up (node, node);

			while (tnode) {
				draw_item (line -=
						   draw_item (0, 0, tnode, drawmode_test), 0, tnode,
						   drawmode_normal);

				line_nodeno[line] = node_no (tnode);

				if (node_down (tnode) == prev_down) {
					hnb_nodes_up++;
					prev_down = tnode;
				}

				tnode = up (node, tnode);
				if (active_line - nodes_above >= line)
					tnode = 0;
			}
		}
/* draw the currently selected item */

		line_nodeno[active_line] = node_no (node);

		if (edit_mode) {
			lines = draw_item (active_line, (int) (intptr_t) input, node, drawmode_edit);
		} else {
			lines =
				draw_item (active_line, strlen (input), node,
						   drawmode_completion);
		}

/* draw items below current item */
		{
			Node *prev_up = node;	/* to aid pgup/pgdn */
			Node *tnode = down (node, node);

			lines += active_line;
			if (lines >= LINES)
				tnode = 0;
			while (tnode) {
				line_nodeno[lines] = node_no (tnode);
				lines += draw_item (lines, 0, tnode, drawmode_normal);

				if (node_up (tnode) == prev_up) {
					hnb_nodes_down++;
					prev_up = tnode;
				}

				tnode = down (node, tnode);
				if (lines >= LINES)
					tnode = 0;

			}
		}
	}

	help_draw (ui_current_scope);

	move (LINES - 1, COLS - 1);

/*	refresh ();*/

	hnb_nodes_up++;
	hnb_nodes_down++;
}

/*
!init_ui_draw();
*/
void init_ui_draw ()
{
	cli_add_command ("display_format", display_format_cmd, "<format string>");
	cli_add_help ("display_format", "\
Defines how each node is displayed. The display string syntax is \
interpreted as follows: \
Spaces become real spaces, i means indentation, - means bullet, \
d means the real data of the node, x is a temporary placeholder for \
upcoming columntypes (for debugging only). \
i and x can take an argument specifying how many characters wide \
the field should be.");
	cli_add_string ("bullet_leaf", bullet_leaf, "Marker for an entry with no sub-entries.");
	cli_add_string ("bullet_parent", bullet_parent, "Marker for a collapsed parent entry.");
	cli_add_string ("bullet_parent_expanded", bullet_parent_expanded, "Marker for an expanded parent entry.");
	cli_add_string ("bullet_todo_parent_undone", bullet_todo_parent_undone, "Marker for an uncompleted todo entry with children.");
	cli_add_string ("bullet_todo_parent_done", bullet_todo_parent_done, "Marker for a completed todo entry with children.");
	cli_add_string ("bullet_todo_leaf_undone", bullet_todo_leaf_undone, "Marker for an uncompleted todo entry with no children.");
	cli_add_string ("bullet_todo_leaf_done", bullet_todo_leaf_done, "Marker for a completed todo entry with no children.");
	cli_add_string ("bullet_textleaf", bullet_textleaf, "Marker for a text entry with no children.");
	cli_add_string ("bullet_textparent", bullet_textparent, "Marker for a text entry with children.");
	cli_add_string ("bullet_textparent_expanded", bullet_textparent_expanded, "");
}
