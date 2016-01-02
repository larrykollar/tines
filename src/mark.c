/*
 * mark.c -- a simplistic save place/return kind of system
 *
 */
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "node.h"
#include "prefs.h"
#include "ui.h"
#include "ui_cli.h"
#include "evilloop.h"

#define MAX_MARK 10
Node *marks[MAX_MARK];	/* for marks 0-9. should be expandable */

static void* set_mark(int argc, char **argv, void *data)
{
    Node *pos=(Node *)data;
    int mark_number;

    if( (argc!=2))
	{
	    cli_outfunf("usage: %s <mark_number>", argv[0]);
	    return data;
	}
    mark_number=atoi(argv[1]);

    if(mark_number <= MAX_MARK)
	{
	    marks[mark_number] = pos;
	}

    return pos;
}

static void* go_mark(int argc, char **argv, void *data)
{
    Node *pos=(Node *)data;
    int mark_number;

    if( (argc!=2))
	{
	    cli_outfunf("usage: %s <mark_number>", argv[0]);
	    return data;
	}
    mark_number=atoi(argv[1]);

    if(mark_number <= MAX_MARK)
	{
	    Node *temp = marks[mark_number];
	    if (temp)
		pos = temp;
	}

    return pos;
}

/*
!init_marks();
*/
void init_marks()
{
    cli_add_command ("set_mark", set_mark,"<mark number>");
    cli_add_help ("set_mark", "define a location for use with 'go_mark'");
    cli_add_command ("go_mark",  go_mark, "<mark_number>");
    cli_add_help ("go_mark", "goto a location saved with 'set_mark'");
}

