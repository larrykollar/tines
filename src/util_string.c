/*
 * util_string.c -- string utility functions for hnb
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int string_isoneof(char *string, char **strings){
	while(*strings){
		if(!strcmp(string,*strings))
			return 1;
		strings++;
	}
	
	return 0;
}


char *string_replace(char *string,  char **mappings ){
	char *newstring;
	
	if(!string)
		return NULL;
		
	/* allocate new string */
	{
		int oldlen=strlen(string);
		int newlen=oldlen;
		char *inpos=string;
		
		while(*inpos){
			int match=0;
			char **cmap=mappings;
			while(*cmap && !match){
				if(!strncmp(inpos,*cmap, strlen(*cmap) )){
					inpos+=strlen(*cmap);
					newlen+=(strlen(*(cmap+1))-strlen(*(cmap)));
					match=1;
				}
				cmap+=2;
			}
			if(!match)
				inpos++;
		}
		newstring=malloc(newlen+1); /* add room for \0 */
	}

	if(!newstring)
		return NULL;

	/* build new string */
	{
		char *inpos=string;
		char *outpos=newstring;
		
		while(*inpos){
			int match=0;
			char **cmap=mappings;
			while(*cmap && !match){
				if(!strncmp(inpos,*cmap, strlen(*cmap) )){
					strcpy(outpos,*(cmap+1));
					outpos+=strlen(*(cmap+1));
					inpos+=strlen(*cmap);
					match=1;
				}
				cmap+=2;
			}
			if(!match){
				*(outpos++)=*(inpos++);
			}
		}
		*outpos='\0';
	}
	
	return newstring;

}

