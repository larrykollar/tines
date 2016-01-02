/*
 * cal.c -- feature to insert a calendar in hnb
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

/**************/
#include "tree.h"
#include "cli.h"
#include "ui.h"
#include "file.h"
#include "prefs.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static char *const wday[] = 
	{ "Sun", "Mon", "Tue",  "Wed", "Thu", "Fri", "Sat", "   "};

static char *const mname[] = {
	  "", "January", "February", "March", "April", "May" ,"June", "July",
	  "August", "September", "October", "November" , "December"};

static void* insert_cal(int argc, char **argv, void *data){
	Node *pos=(void *)data;

	int year;
	int month;
	import_state_t ist;
	
	if( (argc!=3) || (atoi(argv[1])>12 )){
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
				
				/* I prefer not to plan on this level
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
				*/

				tdata.tm_mday++;
				mktime (&tdata);
			}
		}
	}	

	docmd(pos,"tree_changed");
	return pos;
}

/*
!init_cal();
*/
void init_cal(){
	cli_add_command ("insert_cal", insert_cal, "<month> <year>");
}
