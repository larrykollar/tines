#ifndef UI_H
#define UI_H

#include "ui_binding.h"
#include <curses.h>
#include "ui_style.h"
#include "ui_draw.h"
#include "ui_cli.h"

/* actions checked for by hnb.c */

/* must have both of these bound to backspace for some reason*/
/* the third one to work with win32 pdcurses thingy */
/*#define UI_BACKSPACE 127
  #define UI_BACKSPACE2 KEY_BACKSPACE
  #define UI_BACKSPACE3 8mencoder stream.dump -ovc lavc -lavcopts vcodec=mpeg4:vpass=2:vbitrate=905:vhq:v4mv:keyint=120 -oac copy -o spawn_p2.avi
*/

/* initializes the curses ui sets keyboard mode etc.
*/
void ui_init ();

/* returns the terminal to the state it had before ui_init
*/
void ui_end ();


/* waits until a curses event occurs, translates it according
   to the defines above in this file, and returns the new
   value. (also processes some of the events like resize itself)
*/
int ui_input ();

/* used for approximately guessing how much we should skip 
   when page up/page down is requested.
*/
extern long hnb_edit_posup;
extern long hnb_edit_posdown;

extern int ui_inited;

void ui_getstr (char *prompt, char *datastorage);

#define undefined_key(a,c)\
	{if(c!=ui_action_ignore){\
	docmdf(pos,"status \"No action assigned to '%s'(%id) in %s-mode\"",tidy_keyname(keyname(c)),c,a);\
	}}\

#define info(a)\
	{set_status(a);\
	}\

#define infof(a,b)\
	{char msg[80];\
	sprintf(msg,a,b);\
	set_status(msg);\
	}\

#ifdef WIN32
#undef undefined_key
	/*pdcurses in windows defines control keys etc.. flashing errors is not nice */
#define undefined_key(a,c)
#endif

#endif /* UI_H */
