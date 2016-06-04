/*
 * cal.c -- feature to insert a calendar in tines
 *
 * Copyright (C) 2003 Øyvind Kolås <pippin@users.sourceforge.net>
 * Modified for Tines by Larry Kollar, 2016
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

/**************/
#include "tree.h"
#include "cli.h"
#include "ui.h"
#include "file.h"
#include "prefs.h"
#include "tree.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

static char *const wday[] = 
	{ "Sun", "Mon", "Tue",  "Wed", "Thu", "Fri", "Sat", "   "};

static char *const mname[] = {
	  "", "January", "February", "March", "April", "May" ,"June", "July",
	  "August", "September", "October", "November" , "December"};

static void* insert_cal(int argc, char **argv, void *data) {
	Node *pos=(void *)data;

	/* argv[1] = month
	 * argv[2] = year
	 * argv[3] = -t to include workday hourly entries (option)
	 */

	int year;
	int month;
	import_state_t ist;
	
	if( (argc<3) || (atoi(argv[1])>12 )){
		cli_outfunf("usage: %s <month> <year>", argv[0]);
		return data;
	}

	month=atoi(argv[1]);
	year=atoi(argv[2]);

	if(prefs.readonly){
		cli_outfun("readonly flag set, avoiding insertion");
		return data;
	}

	
	if(year<2000){  /* nasty,.. y2k like fix,.. but,.. it's just here */
		year+=2000;
	}
	
	init_import(&ist, pos);
	{
		char tmpstr[40];
		sprintf(tmpstr,"%i %s", year, mname[month]);
		import_node_text(&ist, 0, tmpstr);
		{
			struct tm tdata;

			tdata.tm_year = year - 1900;
			tdata.tm_mon = month - 1;
			tdata.tm_mday = 1;
			tdata.tm_hour = 0;
			tdata.tm_min = 0;
			tdata.tm_sec = 1;
			tdata.tm_isdst = -1;

			mktime (&tdata);

			while(tdata.tm_mon==month-1){
				sprintf (tmpstr,"%s%c%02i\n", wday[tdata.tm_wday], (tdata.tm_wday==0 || tdata.tm_wday==6)?'_':' ', tdata.tm_mday);
				import_node_text(&ist, 1, tmpstr);
				
				/* if anyone wants to plan at this level */
				if( (argc>=4) && !strcmp(argv[3], "-t") ) {
					import_node_text(&ist, 2, "08:00");
					import_node_text(&ist, 2, "09:00");
					import_node_text(&ist, 2, "10:00");
					import_node_text(&ist, 2, "11:00");
					import_node_text(&ist, 2, "12:00");
					import_node_text(&ist, 2, "13:00");
					import_node_text(&ist, 2, "14:00");
					import_node_text(&ist, 2, "15:00");
					import_node_text(&ist, 2, "16:00");
					import_node_text(&ist, 2, "17:00");
					import_node_text(&ist, 2, "18:00");
				}

				tdata.tm_mday++;
				mktime (&tdata);
			}
		}
	}	

	docmd(pos,"tree_changed");
	return pos;
}

static void* jump_today(int argc, char **argv, void *data) {
	Node *pos=(void *)data;

	/* ctime returns a string like 'Tue Mar 15 23:58:22 2016\n\0' */
	time_t now = time((time_t *)NULL);
	char *today = ctime(&now);
	char *mo  = today +  4;
	char *yr  = today + 20;
	char *day = today +  8;

	char buf[40];

	pos = node_root(pos); /* go root */

	/* find the month (eg "2016 Mar") */
	(void)strncpy(buf, yr, 4); buf[4] = ' ';
	(void)strncpy(buf+5, mo, 3); buf[8] = '\0';
	pos = node_recursive_match( buf, pos );
	if( pos == NULL ) {
		docmdf( pos, "Calendar for '%s' not found. Use 'insert_cal <mo> <yr>' to create one.", buf );
		return data;
	}

	/* now find the day (eg "Wed 16") */
	(void)strncpy(buf, today, 4); /* includes the space */
	if(!strncmp(today, "Sat", 3) || !strncmp(today, "Sun", 3)) buf[3]='_';
	(void)strncpy(buf+4, day, 2); buf[6] = '\0';
	if(buf[4]==' ') buf[4]='0'; /* day < 10 */
	pos = node_recursive_match( buf, pos );
	if( pos == NULL ) {
		docmdf( pos, "Day '%s' not found. Use 'insert_cal <mo> <yr>' to create a real calendar.", buf );
		return data;
	}

	docmd( pos, "expand --subtree" );
	return(pos);
}

/*
!init_cal();
*/
void init_cal() {
	cli_add_command ("insert_cal", insert_cal, "<month> <year> [-t]");
	cli_add_help("insert_cal", "Inserts a calendar for the specified month. The -t option adds workday hours for each day.");
	cli_add_command("today", jump_today, "");
	cli_add_help("today", "Jumps to today's calendar entry, if it exists.");
}
