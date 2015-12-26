/* libcli -- a small commandline interpreter libraray
 * Copyright (C) 2002 Øyvind Kolås
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <string.h>
#include <stdio.h>

#include "cli.h"

int integer=0;
char string[40]="not set";

int echo(int argc,char **argv, void *data){
	printf("tokens:%i\n",argc);
	while(*argv)
		printf("[%s]\n",*(argv++));
	return 0;
}

int main(){
	char command[100]="";
	
	cli_add_command("echo",echo,"outputs it's parameters");
	cli_add_int("int",&integer,"a integer");
	cli_add_string("string",string,"a string");
	cli_load_file("macrotest");
	
	while( strcmp(command,"quit") && strcmp(command,"q")){
		printf(">");
		fgets(command,100,stdin);
		command[strlen(command)-1]=0; /* remove newline */
		cli_docmd(command,NULL);
	}

	return 0;
}
