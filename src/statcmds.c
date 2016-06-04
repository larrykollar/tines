/*
 * statcmds.c -- commands that output various statistics
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


#include "tree.h"
#include "cli.h"
#include <stdio.h>

#ifndef WIN32

static void* mem_cmd (int argc, char **argv, void *data)
{
	Node *pos = (Node *) data;

	{
		int VmSize;
		FILE *file;

		file = fopen ("/proc/self/stat", "r");
		if (!file)
			return pos;

		fscanf (file,
				"%*i %*s %*s %*i %*i %*i %*i %*i %*i %*i %*i %*i %*i %*i %*i %*i %*i %*i %*i %*i %*i %*i %i",
				&VmSize);
		fclose (file);

		cli_outfunf ("Memory used: %2.2fmb (%ib)",
					 (float) (VmSize / 1024.0 / 1024.0), VmSize);
	}


	{
		int free, buffers, cached;
		FILE *file;

		file = fopen ("/proc/meminfo", "r");
		if (!file)
			return pos;

		fscanf (file, "%*s %*s %*s %*s %*s %*s %*s %*i %*i %i %i %i",
				&free, &buffers, &cached);
		fclose (file);

		cli_outfunf ("Memory free: %2.2fmb (+buffers/cache: %2.2fmb)",
					 (float) (free / 1024.0 / 1024.0),
					 (float) ((free + buffers + cached) / 1024.0 / 1024.0));
	}

	return pos;
}
#endif
/*
!init_mem();
*/
void init_mem ()
{
#ifndef WIN32
	cli_add_command ("mem", mem_cmd, "");
	cli_add_help ("mem", "Reports memory usage.");
#endif
}

#include <ctype.h>

static int count_words (unsigned char *str)
{
	int words = 0;
	unsigned char *p = str;

	do {
		if (!isspace (*p)) {
			words++;
			while (*p && (!isspace (*p)))
				p++;
		} else {
			p++;
		}
	} while (*p);
	return words;
}

static void* stats_cmd (int argc, char **argv, void *data)
{
	int words = 0, leaves = 0, nodes = 0;
	Node *pos = (Node *) data;
	Node *node = node_root (pos);

	while (node) {
		nodes++;
		words += count_words ((unsigned char *)fixnullstring (node_get (node, TEXT)));
		if (!node_right (node))
			leaves++;

		node = node_recurse (node);
	}

	cli_outfunf ("nodes:%i, leaves:%i words:%i", nodes, leaves, words);

	return pos;
}

/*
!init_stats();
*/
void init_stats ()
{
	cli_add_command ("stats", stats_cmd, "");
	cli_add_help ("stats",
		"Displays the number of items, leaf items, and total number of words in the tree.");
}
