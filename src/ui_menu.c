#include "tree.h"
#include "cli.h"
#include "ui.h"
#include "ui_style.h"
#include <string.h>
#include <curses.h>

int ui_menu_is_not_implemented_yet;

typedef struct MenuT{
	char *label;
	char *command;
	long width;
} MenuT;

#define MENU_MAX_COLS 10
#define MENU_MAX_ROWS 30

static MenuT menu[MENU_MAX_COLS][MENU_MAX_ROWS];

static int xpos_forcol(int colno){
	int cumulated=0;
	int col;
	
	for(col=0;col<colno;col++)
		cumulated+=menu[col][0].width+1;
		
	return cumulated;
}

static void draw_menu(Node *pos, int cur_col, int cur_row){
	int row,col;
	ui_draw(pos,"",0);
		
	for(col=0;col<MENU_MAX_COLS;col++)
		for(row=0;row<MENU_MAX_ROWS;row++){
			if((row==0 || col==cur_col) && menu[col][row].label){
				move(row,xpos_forcol(col));
				ui_style(ui_style_menutext);

				if(col!=cur_col+1)
					addch((col==cur_col && row==cur_row)?'>':' ');
				else
					move(row,xpos_forcol(col)+1);

				ui_style(ui_style_menuitem);
				addstr(menu[col][row].label);
				{long i=menu[col][0].width-strlen(menu[col][row].label);
				 while(i--)addch(' ');
				}
				ui_style(ui_style_menutext);

				addch((col==cur_col && row==cur_row)?'<':' ');
			}
		}
		move (LINES - 1, COLS - 1);	
}

static Node *menu_do(Node *pos){
	static int row=0,col=0;
	int stop=0;
	Tbinding *c;

	while(!stop){
		draw_menu(pos,col,row);	
		c=parsekey(ui_input(),ui_scope_nodeedit);
		switch(c->action){
			case ui_action_right:
				if(menu[col+1][0].label)
					col++;
				else
					col=0;
				while(!menu[col][row].label)
					row--;
				break;
			case ui_action_left:
				if(col>0)
					col--;
				else
					while(menu[col+1][0].label)
						col++;
				while(!menu[col][row].label)
					row--;
				break;
			case ui_action_up:
				if(row>1)
					row--;
				else
					while(menu[col][row+1].label)
						row++;
				break;
			case ui_action_down:
				if(menu[col][row+1].label)
					row++;
				else
					while(row>1)
						row--;				
				break;
			case ui_action_cancel:
				stop=1;
				break;
			case ui_action_confirm:
				if(row)pos=docmd(pos,menu[col][row].command);
				stop=1;
				break;
			default:
				break;
		}
	}
	return pos;
}

static void* cmd_menu(int argc,char **argv,void *data){
	Node *pos=(Node *)data;
    
	if(argc==1){ /* draw menu */
		return menu_do(pos);
	} else if(argc==4){
		char *submenu=argv[1];
		char *label=argv[2];
		char *command=argv[3];
		int col=0,row=0;

		/* locate right submenu */
		while(col<MENU_MAX_COLS){
			if(menu[col][row].label && !strcmp(menu[col][row].label,submenu))
				break;
			col++;
		}
		if(col==MENU_MAX_COLS){
			col=0;
			while(menu[col][row].label)
				col++;
			menu[col][0].label=strdup(submenu);
			menu[col][0].width=strlen(submenu);
		}
		
		while(menu[col][row].label)
			row++;
		menu[col][row].label=strdup(label);
		menu[col][row].command=strdup(command);
		menu[col][row].width=strlen(label);
		
		if(menu[col][row].width>menu[col][0].width)
			menu[col][0].width=menu[col][row].width;	
	}
	return pos;
}

/*
!init_ui_menu();
*/
void init_ui_menu(void){
	cli_add_command("menu", cmd_menu, "<menu name> <entry text> <cmd>");
	cli_add_help("menu",
		"Adds the specified menu entry and associated command or macro.");
	memset(menu,0,sizeof(menu));
}

#if 0

macro define <name>
	commandline 1
	commandline 2
	commandline 3
	commandline 4
end

menu file "save (F2)" "save"
menu file "revert" "revert"
menu file "quit" "quit"

menu edit "copy ^C" "copy"
menu edit "cut ^X" "cut"
menu edit "paste ^V" "paste"

#endif
