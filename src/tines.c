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
#include <errno.h>
#include <locale.h>
#include <getopt.h>

#include "tree.h"

#include "ui_cli.h"
#include "cli.h"
#include "ui.h"
#include "prefs.h"
#include "file.h"
#include "evilloop.h"
#include "file_copy.h"


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
\t-rc=<file>        specify a different config file\n\
\t-ui=<interface>   interface to use, curses (default) or CLI\n\
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
		char format[PREFS_FMT_LEN];
		int ui;
		int tutorial;
		char *dbfile;
		char *rcfile;
		char *cmd;
	} cmdline = {
			0,					/* version */
			0,					/* usage */
			1,					/* load default db */
			"",					/* format to load by default */
			1,					/* ui */
			0,					/* tutorial */
	NULL, NULL, NULL};

	/* args for getopt_long() */
	int ch;
	char progname[PREFS_FN_LEN];

	static struct option longopts[] = {
		{ "help",		no_argument,	NULL,	'h' },
		{ "version",	no_argument,	NULL,	'v' },
		{ "tutorial",	no_argument,	NULL,	't' },
		{ "ascii",		no_argument,	NULL,	'a' },
		{ "hnb",		no_argument,	NULL,	'H' },
		{ "opml",		no_argument,	NULL,	'o' },
		{ "xml",		no_argument,	NULL,	'x' },
		{ "ui",		required_argument,	NULL,	'u' },
		{ "rc",		required_argument,	NULL,	'r' },
		{ NULL,			0,				NULL,	0 }
	};

	strcpy(progname, argv[0]);

	while ((ch = getopt_long( argc, argv, "hvtaoxe", longopts, NULL )) != -1) {
		switch(ch) {
		case 'h':
			cmdline.usage = 1;
			usage (progname);
			exit (0);
		case 'v':
			cmdline.version = 1;
			fprintf (stderr, "%s %s\n", PACKAGE, VERSION);
			exit (0);
		case 't':
			cmdline.tutorial = 1;
			break;
		case 'a':
			strcpy(cmdline.format, "ascii");
			break;
		case 'H':
			strcpy(cmdline.format, "hnb");
			break;
		case 'o':
			strcpy(cmdline.format, "opml");
			break;
		case 'x':
			strcpy(cmdline.format, "xml");
			break;
		case 'u':
			if(!strcmp(optarg, "curses")) {
				cmdline.ui = 1;
			} else if(!strcmp(optarg, "cli")) {
				cmdline.ui = 2;
			} else if(!strcmp(optarg, "keygrab")) {
				cmdline.ui = 4;
			} else {
				fprintf(stderr, "unknown -ui option: %s\n", optarg);
				usage( progname );
				exit(1);
			}
			break;
		case 'r':
			cmdline.rcfile = malloc(strlen(optarg)+1);
			strcpy(cmdline.rcfile, optarg);
			break;
		case 'e': /* one-liner ala sed */
			if(!cmdline.dbfile) {
				cmdline.def_db = 1;
				cmdline.dbfile = (char *) -1;
			}
			break;
		case ':':
			fprintf(stderr, "Missing value for --rc or --ui option\n");
			usage( progname );
			exit(1);
		case '?':
		default:
			fprintf(stderr, "unknown option\n");
			usage( progname );
			exit(1);
		}
	}
	/* anything left is either a file name or -e commands */
	if(!cmdline.dbfile) {
		if( argc > optind ) {
			cmdline.dbfile = argv[optind];
			cmdline.def_db = 0;
		}
	} else {
		cmdline.cmd = argv[optind++];
		cmdline.ui = 0;
	}

	prefs.ui = cmdline.ui;
	argno = optind; /* pick up anything left on the command line */


	init_subsystems ();

	setlocale(LC_ALL, "");

	/* init has set prefs.rc_file & prefs.default_db_file */

	/* rc file logic:
	 * 1) --rc specifed on command line?
	 * 2) No (or it doesn't exist): ~/.tinesrc exists?
	 * 3) No: try copying SHAREDIR/RCFILEIN (to named rc file if named)
	 * 4) Step 3 fails: Generate minimal rc file (to named rc file if named)
	 */
	if (cmdline.rcfile && (strlen(cmdline.rcfile)<PREFS_FN_LEN)
			&& file_check(cmdline.rcfile) ) {
		strcpy (prefs.rc_file, cmdline.rcfile);
	}

	/* no --rc or bad file. If no .tinesrc, create it */
	if (!file_check (prefs.rc_file)) {
		/* No .tinesrc... see if there's a sharedir */
		char defpref[PREFS_FN_LEN];
		snprintf(defpref, PREFS_FN_LEN, "%s%s", SHAREDIR, RCFILEIN);
		if( file_check(defpref) ) {
			cp_file(prefs.rc_file, defpref);
			fprintf( stderr, "Created %s from %s\n", prefs.rc_file, defpref );
		} else {
			write_default_prefs ();
			fprintf (stderr, "Created %s from fallback prefs\n",
					 prefs.rc_file);
		}
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
		if( strlen(prefs.default_db_file)<PREFS_FN_LEN )
			strcpy (prefs.db_file, prefs.default_db_file);
		if (!file_check (prefs.db_file))
			prefs.tutorial = 2;
	} else {
		strcpy (prefs.db_file, cmdline.dbfile);
	}

	pos = tree_new ();

	if (!prefs.tutorial) {
		int oldpos = -1;
		char file_to_load[PREFS_FN_LEN];

		strcpy(file_to_load, prefs.db_file);
		
		{ /* check for recovery file */
		  char recovery_file[PREFS_FN_LEN];
		  FILE *tfile;
		  
	  	  struct stat statbuf;
		  long file_modified;
	   	  stat(prefs.db_file, &statbuf);
		  file_modified=statbuf.st_ctime;
		  
		  snprintf(recovery_file, PREFS_FN_LEN, "%s_tines_rescue", prefs.db_file);
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
				  fprintf(stderr, "A recovery file (%s) exists.\n\
This could mean that a prior instance of Tines is still running\n\
or that tines was aborted.\n", recovery_file);
				  if(rescue_modified<file_modified)
					  fprintf(stderr, "\n!!NOTE: original is newer than recovery, be careful.\n");
				  fprintf(stderr, "\n\
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
		    !strcmp(prefs.format,"opml") ||
		    !strcmp(prefs.format,"hnb") || 
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
			char buf[PREFS_FN_LEN];
			if(recover)
				sprintf (buf, "import_binary %s", file_to_load);
			else {
				/* File specified on command line:
				 *   exists: load it
				 *   does not exist: empty tree
				 * File not specified on command line:
				 *   ~/.tines exists: load it
				 *   does not exist: load the starter db or a mini. */
				if(file_check(file_to_load)) {
					sprintf (buf, "import_%s \'%s\'", prefs.format, file_to_load);
				} else {
					char tmp[MAXPATHLEN];

					sprintf (tmp, "%s%s", SHAREDIR, DATFILEIN);
					if(file_check(tmp) && !cmdline.dbfile) {
						/* copy default DB */
						if(cp_file(file_to_load, tmp) == -1)
							fprintf(stderr, "File copy error: %d\n", errno);
					}
					sprintf (buf, "import_%s \'%s\'", prefs.format, file_to_load);
				}
			}

			pos = docmd (pos, buf);
		}

		if (oldpos != -1) {
			while (oldpos--)
				pos = node_recurse (pos);
		}
	}

	if (prefs.tutorial) {
		char tmp[MAXPATHLEN];
		char buf[MAXPATHLEN];

		sprintf (tmp, "%s%s", SHAREDIR, DATFILEIN);
		if(file_check(tmp)) {
			/* copy default DB */
			if(cp_file(prefs.db_file, tmp) == -1)
				fprintf(stderr, "File copy error: %d\n", errno);
			sprintf (buf, "import_%s \'%s\'", prefs.format, prefs.db_file);
			fprintf(stderr, "Created default db %s from %s\n", prefs.db_file, tmp);
			pos = docmd (pos, buf);
			pos = docmd (pos, "status 'Loaded default welcome database.'");
		} else {
			if (prefs.tutorial != 2)
				prefs.db_file[0] = (char) 255;	/* disable saving */
			pos = docmd (pos, "import_help");
			pos = docmd (pos, "status ''");
			pos = docmd (pos, "status 'Navigate the documentation with your cursor keys.'");
		}
	}

	switch (cmdline.ui) {
		case 1: /* --ui=curses */
			pos = evilloop (pos);
			ui_end ();
			break;
		case 0: /* -e */
			pos = (Node *) cli_docmd (cmdline.cmd, pos);
			while (argno < argc) {
				pos = (Node *) cli_docmd (argv[argno++], pos);
			}
			break;
		case 2: /* --ui=cli */
			pos = cli (pos);
			break;
		case 3: /* --ui=(undefined) */
			printf ("gtk+ interface not implemented\n");
			break;
		case 4: /* --ui=keygrab */
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
	    char swapfile[PREFS_FN_LEN+15];
	    sprintf(swapfile,"%s_tines_rescue",prefs.db_file);
		if(!prefs.readonly)
			    remove(swapfile);
    }

	return 0;
}
