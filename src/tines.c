/*
 * tines.c -- the main loop of tines, an outliner/planner/organizer/notebook
 *
 * Copyright (C) 2001-2003 Øyvind Kolås <pippin@users.sourceforge.net>
 * hnb forked by Larry Kollar, Dec 2015, renamed Tines
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

/*
	TODO: noder som forsvinner ved:
		std. oppretting
		redigering
		gå til parent
		
		--
		sannsynlig grunn: feil håndtering av temporary attributte

 (Google Translate says this means:

 nodes which disappear [which I haven't seen -LK]
 	std. creation
	editing
	go to parent

	likely reason: incorrect handing of temporary attributes)
*/

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <locale.h>

#include "tree.h"

#include "ui_cli.h"
#include "cli.h"
#include "ui.h"
#include "prefs.h"
#include "file.h"
#include "evilloop.h"

static void usage (const char *av0)
{
	fprintf (stderr,
			 "\nusage: %s [database] [options] [command [command] ..]\n",
			 av0);
	fprintf (stderr, "\n\
Tines by Larry Kollar (lkollar@gmail.com), a fork of hnb by Oyvind Kolas\n\
It is distributed under the GNU General Public License.\n\
\n\
default database: '%s'\n", prefs.default_db_file);
	fprintf (stderr, "\n\
Options:\n\
\n\
\t-h --help     this message\n\
\t-v --version  prints the version\n\
\t-t --tutorial loads the tutorial instead of a database\n\
\n\
\t-a --ascii    load indented ascii\n\
\t   --hnb      load hnb file\n\
\t-o --opml     load OPML file\n\
\t-x --xml      load general xml file\n");
#ifdef USE_LIBXML
	fprintf (stderr, "\t-s --stylized load stylized xml (using libxml2)\n");
#endif
	fprintf (stderr, "\n\
\t-rc <file>        specify a different config file\n\
\t-ui <interface>   interface to use, curses (default) or CLI\n\
\t-e                execute commands\n\
\n\n");
}

void init_subsystems ();


int main (int argc, char **argv)
{
	Node *pos;
	int argno;

	/* current commandline argument in focus */
	
	int recover=0;  /* whether in recover mode */

	struct {					/* initialized defaults */
		int version;
		int usage;
		int def_db;
		char format[64];
		int ui;
		int tutorial;
		char *dbfile;
		char *rcfile;
		char *cmd;
	} cmdline = {
			0,					/* version */
			0,					/* usage */
			1,					/* load default db */
			"",					/*format to load by default */
			1,					/* ui */
			0,					/* tutorial */
	NULL, NULL, NULL};

/* TODO: replace this with getopt_long() (getopt.h) */
	{							/*parse commandline */
		for (argno = 1; argno < argc; argno++) {
			if (!strcmp (argv[argno], "-h")
				|| !strcmp (argv[argno], "--help")) {
				cmdline.usage = 1;
			} else if (!strcmp (argv[argno], "-v")
					   || !strcmp (argv[argno], "--version")) {
				cmdline.version = 1;
			} else if (!strcmp (argv[argno], "-t")
					   || !strcmp (argv[argno], "--tutorial")) {
				cmdline.tutorial = 1;
			} else if (!strcmp (argv[argno], "-a")
					   || !strcmp (argv[argno], "--ascii")) {
				strcpy(cmdline.format,"ascii");
			} else if (!strcmp (argv[argno], "-hnb")
					   || !strcmp (argv[argno], "--hnb")) {
				strcpy(cmdline.format,"hnb");
			} else if (!strcmp (argv[argno], "-o")
					   || !strcmp (argv[argno], "-opml")
					   || !strcmp (argv[argno], "--opml")) {
				strcpy(cmdline.format,"opml");
			} else if (!strcmp (argv[argno], "-x")
					   || !strcmp (argv[argno], "-gx")
					   || !strcmp (argv[argno], "--xml")) {
				strcpy(cmdline.format,"xml");
#ifdef USE_LIBXML
			} else if (!strcmp (argv[argno], "-s")
					   || !strcmp (argv[argno], "-sx")
					   || !strcmp (argv[argno], "--stylized")) {
				strcpy(cmdline.format,"sxml");
#endif
			} else if (!strcmp (argv[argno], "-ui")) {
				if (!strcmp (argv[++argno], "curses")) {
					cmdline.ui = 1;
				} else if (!strcmp (argv[argno], "cli")) {
					cmdline.ui = 2;
				} else if (!strcmp (argv[argno], "gtk")
						   || !strcmp (argv[argno], "gtk+")) {
					cmdline.ui = 3;
				} else if (!strcmp (argv[argno], "keygrab")) {
					cmdline.ui = 4;
				} else {
					fprintf (stderr, "unknown interface %s\n", argv[argno]);
					exit (1);
				}
			} else if (!strcmp (argv[argno], "-rc")) {
				cmdline.rcfile = argv[++argno];
			} else if (!strcmp (argv[argno], "-e")) {
				/* actually just a dummy option to specify default db */
				if (!cmdline.dbfile) {
					cmdline.def_db = 1;
					cmdline.dbfile = (char *) -1;
				}
			} else {
				if (argv[argno][0] == '-') {
					fprintf (stderr, "unknown option %s\n", argv[argno]);
					exit (1);
				} else if (!cmdline.dbfile) {
					cmdline.dbfile = argv[argno];
					cmdline.def_db = 0;
				} else {
					cmdline.cmd = argv[argno];
					cmdline.ui = 0;
					argno++;
					break;		/* stop processing cmd args */
				}
			}
		}
	}

	init_subsystems ();

	setlocale(LC_ALL, "");

	if (cmdline.usage) {
		usage (argv[0]);
		exit (0);
	}

	if (cmdline.version) {
		fprintf (stderr, "%s %s\n", PACKAGE, VERSION);
		exit (0);
	}

	if (cmdline.rcfile) {
		strcpy (prefs.rc_file, cmdline.rcfile);
	}


	if (!file_check (prefs.rc_file)) {
		write_default_prefs ();
		fprintf (stderr, "created %s for hnb preferences file\n",
				 prefs.rc_file);
		sleep (1);
	}

	if (cmdline.ui == 1)
		ui_init ();

	load_prefs ();


	/* ovveride the prefs with commandline specified options */
	if (cmdline.tutorial)
		prefs.tutorial = 1;
	if (cmdline.format[0] ) {	/* format specified */
		strcpy(prefs.format, cmdline.format);
	}

	if (cmdline.def_db) {
		strcpy (prefs.db_file, prefs.default_db_file);
		if (!file_check (prefs.db_file))
			prefs.tutorial = 2;
	} else {
		strcpy (prefs.db_file, cmdline.dbfile);
	}

	pos = tree_new ();

	if (!prefs.tutorial) {
		int oldpos = -1;
		char file_to_load[4096]; /* TODO: malloc this */

		strcpy(file_to_load, prefs.db_file);	
		
		{ /* check for recovery file */
		  char recovery_file[4096]; /* TODO: malloc this */
		  FILE *tfile;
		  
	  	  struct stat statbuf;
		  long file_modified;
	   	  stat(prefs.db_file, &statbuf);
		  file_modified=statbuf.st_ctime;
		  
		  sprintf(recovery_file, "%s_hnb_rescue", prefs.db_file);
		  tfile = fopen(recovery_file, "r");
		  if(tfile){
			  char response[1024]="_";
			  int got_response=0;
			  long rescue_modified;
			  int ui_was_inited=ui_inited;
	   		  stat(recovery_file, &statbuf);
			  rescue_modified=statbuf.st_ctime;

			  if(ui_inited)
				  ui_end();
	
			  fclose(tfile);
			 while(!got_response){
				  fprintf(stderr,"hnb recovery file (%s) exists\n\
This could mean that a prior instance of hnb is still running or that hnb\n\
was aborted.\n", recovery_file);
				  if(rescue_modified<file_modified)
					  fprintf(stderr,"\n!!NOTE: original is newer than recovery, be careful.\n");
				  fprintf(stderr,"\n\
a)bort\n\
d)elete recovery file\n\
r)recover\n\
o)pen read_only\n\
[d|r|o]>");
			  	fgets(response,10,stdin);
				switch(response[0]){
					case 'a':
						return 0;
						break;
					case 'd':
						remove(recovery_file);
						got_response=1;
						break;
					case 'r':
						strcpy(file_to_load, recovery_file);
						recover=1;
						got_response=1;
						break;
					case 'o':
						got_response = 1;
						prefs.readonly = 1;
						break;
					default:
						break;
				}
			 }
			 if(ui_was_inited)
				 ui_init();
		  }
		}
		
		if (!recover && ( 
		    !strcmp(prefs.format,"hnb") || 
		    !strcmp(prefs.format,"opml") ||
		    !strcmp(prefs.format,"xml") )
		   ) {

			if (!xml_check (file_to_load)) {
				fprintf (stderr,
						 "%s does not seem to be a xml file, aborting.\n",
						 file_to_load);
				if (ui_inited)
					ui_end ();
				exit (1);
			}
			if (prefs.savepos)
				oldpos = xml_getpos (file_to_load);
		}


		{
			char buf[4096]; /* TODO: malloc this */
			if(recover)
			sprintf (buf, "import_binary %s", file_to_load);
			else
			sprintf (buf, "import_%s %s", prefs.format,  file_to_load);

			pos = docmd (pos, buf);
		}

		if (oldpos != -1) {
			while (oldpos--)
				pos = node_recurse (pos);
		}
	}

/* TODO: this should load a default read-only db from SHAREDIR
 * and set the readonly flag.
 */
	if (prefs.tutorial) {
		if (prefs.tutorial != 2)
			prefs.db_file[0] = (char) 255;	/* disable saving */
		pos = docmd (pos, "import_help");
		pos = docmd (pos, "status ''");
		pos = docmd (pos, "status 'navigate the documentation with your cursor keys'");
	}

	switch (cmdline.ui) {
		case 1:
			pos = evilloop (pos);
			ui_end ();
			break;
		case 0:
			pos = (Node *) cli_docmd (cmdline.cmd, pos);
			while (argno < argc) {
				pos = (Node *) cli_docmd (argv[argno++], pos);
			}
			break;
		case 2:
			pos = cli (pos);
			break;
		case 3:
			printf ("gtk+ interface not implemented\n");
			break;
		case 4:
			ui_init ();
			{
				int c = 0;

				while (c != 'q') {
					char buf[100]; /* TODO: malloc this? */

					c = getch ();
					sprintf (buf, "[%i] [%c]\n", c, c);
					addstr (buf);
			}}
			ui_end ();
			break;
	}

	cli_cleanup ();
	tree_free (pos);

	{
	    char swapfile[4096]; /* TODO: malloc this */
	    sprintf(swapfile,"%s_hnb_rescue",prefs.db_file);
		if(!prefs.readonly)
			    remove(swapfile);
    }

	return 0;
}
