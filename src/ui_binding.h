
#ifndef UI_BINDING_H
#define UI_BINDING_H

typedef struct {
	int key;
	int action;
	char *action_name;
	char *action_param;
} Tbinding;


enum {
	ui_scope_main,
	ui_scope_confirm,
	ui_scope_nodeedit,
	ui_scope_lineedit,
	ui_scope_tempterm
};

enum {
	ui_action_backspace = 512,
	ui_action_bkill_word,
	ui_action_bol,
	ui_action_bottom,
	ui_action_bskipword,
	ui_action_cancel,
	ui_action_command,
	ui_action_complete,
	ui_action_confirm,
	ui_action_delete,
	ui_action_down,
	ui_action_eol,
	ui_action_ignore,
	ui_action_join,
	ui_action_kill_line,
	ui_action_kill_to_bol,
	ui_action_kill_to_eol,
	ui_action_kill_word,
	ui_action_left,
	ui_action_pagedown,
	ui_action_pageup,
	ui_action_quit,
	ui_action_right,
	ui_action_skipword,
	ui_action_split,
	ui_action_top,
	ui_action_unbound,
	ui_action_up,
	ui_action_terminator
};

#define MAX_SCOPES 16

extern char *ui_scope_names[MAX_SCOPES];

extern Tbinding *lastbinding;

extern int ui_current_scope;


Tbinding *parsekey (int key, int scope);
char *tidy_keyname (const char *keyname);
char *resolve_binding (int scope, int action);

#endif /* UI_BINDING_H */
