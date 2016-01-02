/*
 * file_ps.c -- postscript export filter for hnb
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

static char *psquote[]={
	"(", "\\(",
	")", "\\)",
	"æ", "\\346",
	"ø", "\\370",
	"å", "\\345",
	"Æ", "\\306",
	"Ø", "\\330",
	"Å", "\\305",NULL
};

/* *INDENT-ON* */

static void ps_export_node (FILE * file, int level, Node *tnode)
{
	char *data = fixnullstring (node_get (tnode, TEXT));
	char *type = node_get (tnode, "type");
	char *quoted=string_replace(data,psquote);
	int done;

	indent (level, "\t");
	if (type && !strcmp (type, "todo")) {
	  type = node_get (tnode, "done");
	  done = type ? type[0] == 'y' : 0;
	  fprintf (file, "( ) S 10 ss %i LM 0 a ([%s]%s ) P\n", level * 22,
		   done ? "DONE" : "____", quoted);
	} else {
	  fprintf (file, "( ) S 10 ss %i LM 0 a (%s ) P\n", level * 22,
			 quoted);
	}
	free(quoted);
}

static void* export_ps (int argc, char **argv, void *data)
{
	Node *node = (Node *) data;
	char *filename = argc==2?argv[1]:"";
	Node *tnode;
	int level, /* flags, */ startlevel;
/*	char *cdata; */
	FILE *file;

	if (!strcmp (filename, "-"))
		file = stdout;
	else
		file = fopen (filename, "w");
	if (!file) {
		cli_outfunf ("postscript export, unable to open \"%s\"", filename);
		return node;
	}
	startlevel = nodes_left (node);

	tnode = node;

	fprintf (file, "%%!PS %% all files must open with this header\n\
%%%%BeginResource minidict.ps\n\
%%%%Creator: byram@cappella-archive.com\n\
%%%%For: Direct PostScript Mark-up\n\
%%%%Date: 24 Feb 2002\n\
%%%%EndComments\n\
/_Z { /defaults save def } def\n\
/ZZ { defaults restore } def\n\
/PW { /pw exch def } def 595 PW %% paper width\n\
/PH { /ph exch def } def 842 PH %% paper height\n\
%% Defaults: tm = text height: rm = text width\n\
/FM { /fm exch def } def 72 FM %% footer margin\n\
/TM { /tm exch def } def 680 TM %% top margin\n\
/RM { /rm exch def } def 480 RM %% right margin\n\
/BM { /bm exch def } def 0 BM %% bottom margin\n\
/LM { /lm exch def } def 0 LM %% left margin\n\
/LG { /lg exch def } def 11 LG %% linespacing\n\
\n\
/PG { /pg exch def } def 1 PG %% first number\n\
/margins { 680 TM 480 RM 0 BM 0 LM } def\n\
\n\
/MF {	%% fontname newfontname -> -	make a new encoded font \n\
/newfontname exch def /fontname exch def /fontdict fontname findfont def /newfont fontdict\n\
maxlength dict def fontdict { exch dup /FID eq { pop pop } { exch newfont 3 1\n\
roll put } ifelse } forall newfont /FontName newfontname put encoding_vector\n\
length 256 eq { newfont /Encoding encoding_vector put } if newfontname newfont\n\
definefont pop } def\n\
\n\
/encoding_vector [ /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
	/.notdef  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
	/.notdef  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
	/.notdef  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
	/.notdef  /.notdef /space /exclam /quotedbl /numbersign /dollar /percent\n\
	/ampersand  /quoteright /parenleft /parenright /asterisk /plus /comma /hyphen\n\
	/period  /slash /zero /one /two /three /four /five /six  /seven /eight /nine\n\
	/colon /semicolon /less /equal /greater  /question /at /A /B /C /D /E /F  /G /H\n\
	/I /J /K /L /M /N  /O /P /Q /R /S /T /U /V  /W /X /Y /Z /bracketleft /backslash\n\
	/bracketright /asciicircum  /underscore /quoteleft /a /b /c /d /e /f  /g /h /i\n\
	/j /k /l /m /n  /o /p /q /r /s /t /u /v  /w /x /y /z /braceleft /bar\n\
	/braceright /tilde  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
	/.notdef /.notdef  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
	/.notdef /.notdef  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
	/.notdef /.notdef  /.notdef /.notdef /.notdef /.notdef /.notdef /.notdef\n\
	/.notdef /.notdef  /.notdef /space /exclamdown /cent /sterling /currency /yen\n\
	/brokenbar  /section /dieresis /copyright /ordfeminine /guillemotleft\n\
	/logicalnot /hyphen /registered  /macron /degree /plusminus /twosuperior\n\
	/threesuperior /acute /mu /paragraph  /bullet /cedilla /onesuperior\n\
	/ordmasculine /guillemotright /onequarter /onehalf /threequarters /questiondown\n\
	/Agrave /Aacute /Acircumflex /Atilde /Adieresis /Aring /AE  /Ccedilla /Egrave\n\
	/Eacute /Ecircumflex /Edieresis /Igrave /Iacute /Icircumflex  /Idieresis /Eth\n\
	/Ntilde /Ograve /Oacute /Ocircumflex /Otilde /Odieresis  /multiply /Oslash\n\
	/Ugrave /Uacute /Ucircumflex /Udieresis /Yacute /Thorn  /germandbls /agrave\n\
	/aacute /acircumflex /atilde /adieresis /aring /ae  /ccedilla /egrave /eacute\n\
	/ecircumflex /edieresis /igrave /iacute /icircumflex  /idieresis /eth /ntilde\n\
	/ograve /oacute /ocircumflex /otilde /odieresis  /divide /oslash /ugrave\n\
	/uacute /ucircumflex /udieresis /yacute /thorn  /ydieresis \n\
] def\n\
\n\
/Times-Roman /Times-Roman-IL1 MF\n\
/Times-Italic /Times-Italic-IL1 MF\n\
/Times-Bold /Times-Bold-IL1 MF\n\
/Helvetica /Helvetica-IL1 MF\n\
/Helvetica-Oblique /Helvetica-Oblique-IL1 MF\n\
/Helvetica-Bold /Helvetica-Bold-IL1 MF\n\
\n\
/textbox {\n\
margins 10 rom 12 LG lm tm moveto } def\n\
/midpage { pw rm sub 2 div fm translate\n\
textbox numbering 10 ss} def\n\
/1upA4 { _Z minidict begin\n\
/p1 { gsave midpage\n\
/jump { bm tm gt { showpage grestore p1 } if\n\
} def } def p1\n\
} def\n\
/newpage { 10 neg TM tm pop jump } def\n\
%% close file\n\
/close { showpage grestore clear end ZZ } def\n\
/minidict 90 dict def \n\
minidict begin \n\
/gs /gsave load def \n\
/gr /grestore load def \n\
/a { tm exch sub TM lm tm moveto } bind def\n\
/Q { lg 8 div a } def\n\
/H { lg 2 div a } def\n\
/L { lg a } def \n\
/centre { dup stringwidth pop 2 div rm 2 div\n\
exch sub lm add tm moveto } bind def\n\
/numbering { gs rm 2 div bm 25 sub moveto\n\
pg pg 1 add PG 4 string cvs 7 ss show gr } def\n\
/find { search { pop 3 -1 roll 1 add 3 1 roll }\n\
{ pop exit } ifelse } def\n\
/spacecount { 0 exch ( ) { find } loop } def\n\
/toofar? { ( ) search pop dup stringwidth pop\n\
currentpoint pop add rm gt } bind def\n\
/pagejump { jump toofar? { L jump s s }\n\
{ s s } ifelse } def\n\
/s /show load def %% \n\
/n { bm tm gt { jump s L } { s L } ifelse} bind def\n\
/c { centre s L } def \n\
/S { dup spacecount { pagejump } repeat pop } def\n\
/P { S L } bind def \n\
/J { P } def \n\
/T { lm pop tm moveto } def %% tabs\n\
/F { findfont exch scalefont setfont } bind def\n\
/rom { /Times-Roman-IL1 F } def\n\
/it { /Times-Italic-IL1 F } def\n\
/bol { /Times-Bold-IL1 F } def\n\
/ss { /Helvetica-IL1 F } def\n\
/si { /Helvetica-Oblique-IL1 } def\n\
/sb { /Helvetica-Bold-IL1 F } def\n\
\n\
end %% close minidict: *this is important*\n\
%%%% EndResource\n\
\n\
\n\
%%%%BeginPageSetup \n\
1upA4 %% open the page \n\
%%%%EndPageSetup \n\
0 a\n\
\n\
%%%%BeginScript \n\
12 si 0 a\n\
12 sb  (hnb postscript export) c 11 ss L\n\
");

	while ((tnode != 0) & (nodes_left (tnode) >= startlevel)) {
		level = nodes_left (tnode) - startlevel;
		ps_export_node (file, level, tnode);
		if (node_up (tnode) && node_up (tnode) == node_backrecurse (tnode))
			fprintf (file, " H\n");

		tnode = node_recurse (tnode);
	}

	fprintf (file, "\
%%%%Trailer \n\
close\n\
%%%%EOF\n");

	cli_outfunf ("postscript export, saved output to \"%s\"", filename);
	if (file != stdout)
		fclose (file);
	return node;
}

/*
!init_file_ps();
*/
void init_file_ps ()
{
	cli_add_command ("export_ps", export_ps, "<filename>");
	cli_add_help ("export_ps",
				  "Exports the current node, it's siblings and all sublevels to a postscript file suitable for printing");
}
