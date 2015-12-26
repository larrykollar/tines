/*
 * file_html.c -- html export filter for hnb
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
#include "query.h"
#include "util_string.h"

#define indent(count,char)	{int j;for(j=0;j<count;j++)fprintf(file,char);}

/* *INDENT-OFF* */

static char *htmlquote[]={
	"&", "&amp;",
	"\"", "&#39;",
	"<", "&lt;",
	">", "&gt;",
	"ø", "&oslash;",
	"Ø", "&Oslash;",
	"å", "&aring;",
	"Å", "&Aring;",
	"æ", "&aelig;",
	"Æ", "&AElig;",NULL
};

/* *INDENT-ON* */


static long export_html (int argc, char **argv, long *data)
{
	Node *node = (Node *) data;
	char *filename = argc==2?argv[1]:"";
	Node *tnode;
	int level, flags, startlevel, lastlevel, cnt;
	char *cdata;
	FILE *file;

	if (!strcmp (filename, "-"))
		file = stdout;
	else
		file = fopen (filename, "w");
	if (!file) {
		cli_outfunf ("html export, unable to open \"%s\"", filename);
		return (long) node;
	}

	startlevel = nodes_left (node);

	tnode = node;
	lastlevel = 0;
	fprintf (file, "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2//EN\">\n\
<html>\n\
<head>\n\
	<title>tree exported from hnb</title>\n\
</head>\n\
<body>\n\
<ul>\n");
	while ((tnode != 0) & (nodes_left (tnode) >= startlevel)) {
		level = nodes_left (tnode) - startlevel;
		flags = node_getflags (tnode);
		cdata = fixnullstring (node_get (tnode, TEXT));

		if (level > lastlevel) {
			indent (level - 1, "\t");
			fprintf (file, "  <ul>\n");
		}

		if (level < lastlevel) {
			int level_diff = lastlevel - level;

			for (; level_diff; level_diff--) {
				indent (level + level_diff - 1, "\t");
				fprintf (file, "  </ul>\n");
			}
		}

		indent (level, "\t");

		if ((strlen(cdata) > 7) && (strncasecmp(cdata, "http://", 7) == 0)) {
			fprintf (file, "<li><a href=\"%s\">%s</a></li>\n", cdata, cdata);
		} else if (cdata[0] != 0) {
			char *quoted=string_replace(cdata,htmlquote);
			fprintf (file, "<li>%s</li>\n", quoted);
			free(quoted);
		} else {
			fprintf (file, "<!-- empty line in input -->\n");
		}

		lastlevel = level;
		tnode = node_recurse (tnode);
	}
	level = 0;

	{
		int level_diff = lastlevel - level;

		for (; level_diff; level_diff--) {
			for (cnt = 0; cnt <= level + level_diff - 1; cnt++)
				fprintf (file, "\t");
			fprintf (file, "  </ul>\n");
		}
	}

	fprintf (file, "</ul>\n</body></html>");
	if (file != stdout)
		fclose (file);

	cli_outfunf ("html export, saved output in \"%s\"", filename);
	return (long) node;
}

static void htmlcss_export_nodes (FILE * file, Node *node, int level)
{
	while (node) {
		char *data = fixnullstring (node_get (node, TEXT));
		char *quoted=string_replace(data,htmlquote);


		fprintf (file, "\n");
		indent (level, "\t");
		fprintf (file, "<div class=\"level%i\">",level+1);
		fprintf (file, "%s", quoted);
		
		free(quoted);


		if (node_right (node)) {
			htmlcss_export_nodes (file, node_right (node), level + 1);
			fprintf (file, "\n");
			indent (level, "\t");
			fprintf (file, "</div>");
		} else {
			fprintf (file, "</div>");
		}

		node = node_down (node);
	}
}


static long export_htmlcss (int argc, char **argv, long *data)
{
	Node *node = (Node *) data;
	char *filename = argc==2?argv[1]:"";
	FILE *file;

	if (!strcmp (filename, "-"))
		file = stdout;
	else
		file = fopen (filename, "w");
	if (!file) {
		cli_outfunf ("html export, unable to open \"%s\"", filename);
		return (long) node;
	}

	fprintf (file, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\
<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \
\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n\
<html><head>\n\
<meta http-equiv=\"Content-type\" content=\"text/html; charset=ISO8859-1\" />\n\
<title>tree exported from hnb</title>\n\
<style type=\"text/css\" id=\"internalStyle\">\n\
body {\n\
	padding-right: 3em;\n\
	padding-left: 3em;\n\
}\n\
div {\n\
	padding-top: 0.5em;\n\
	font-family: verdana, arial, helvetica, sans-serif; position:relative;\n\
	font-size:   10pt;\n\
	left:        2em;\n\
	padding-right: 2em;\n\
}\n\
div.level1 {\n\
	padding-top: 2.5em;\n\
	font-family: verdana, arial, helvetica, sans-serif; position:relative;\n\
	font-size:   18pt;\n\
	text-decoration: underline;\n\
	font-weight: bold;\n\
	left:        0em;\n\
	padding-right: 0em;\n\
}\n\
div.level2 {\n\
	padding-top: 0.5em;\n\
	font-family: verdana, arial, helvetica, sans-serif; position:relative;\n\
	font-size:   12pt;\n\
	text-decoration: none;\n\
	font-weight: bold;\n\
	left:        0em;\n\
	padding-right: 0em;\n\
}\n\
</style>\n\
</head>\n\
<body xmlns=\"http://www.w3.org/1999/xhtml\">\n");

	htmlcss_export_nodes (file, node, 0);

	fprintf (file, "\n</body></html>\n");
	if (file != stdout)
		fclose (file);

	cli_outfunf ("html css export, saved output in \"%s\"", filename);
	return (long) node;
}

/*
!init_file_html();
*/
void init_file_html ()
{
	cli_add_command ("export_html", export_html, "<filename>");
	cli_add_command ("export_htmlcss", export_htmlcss, "<filename>");
}
