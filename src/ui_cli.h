#ifndef CLI_H
#define CLI_H
#include <stdarg.h>
#include "cli.h"

/*	Initialize ui_cli module */
void init_ui_cli (void);


/** execute specified command line as if we stood on pos in the tree

  @returns new position, if position changes due to the action.
*/

Node *docmd (Node *pos, const char *commandline);
Node *docmdf (Node *pos,char *format, ...);


/* enter an read eval loop executing commandlines
   
  Returns: new position, if position changes due to the actions done.
*/
Node *cli (Node *pos);

#endif /* CLI_H */
