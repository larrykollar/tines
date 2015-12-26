#ifndef UI_STYLE_H
#define UI_STYLE_H

enum {
	ui_style_menuitem = 1,
	ui_style_menutext,
	ui_style_node,
	ui_style_parentnode,
	ui_style_bullet,
	ui_style_selected,
	ui_style_parentselected,
	ui_style_background,
	ui_style_terminator
};

void ui_style (int style_no);
void ui_style_restore_color ();

#endif /*UI_STYLE_H */
