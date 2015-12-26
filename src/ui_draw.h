#ifndef UI_DRAW_H
#define UI_DRAW_H

/** draw the interface using curses, current node is node
   *input varies from mode to mode, in GETSTR mode input
   is owerwritten with the value input
*/

void ui_draw (Node *node, char *input, int edit_mode);

void set_status (char *message);
#endif
