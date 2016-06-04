"# Minimal configuration file for Tines\n\
# Comments & default settings deleted to save space in the binary\n\
\n\
display_format \"i4- d\"\n\
\n\
web_command \"lynx *\"\n\
mail_command \"mutt *\"\n\
spell_command \"ispell -x\"\n\
\n\
savepos	\"1\"\n\
saveexpand \"1\"\n\
\n\
autosave_threshold \"15\"\n\
autosave_threshold_nodes_changed \"0\"\n\
autosave_timeout    \"50\"\n\
autosave_timer     \"0\"\n\
\n\
style menuitem       default/default reverse\n\
style menutext       blue/default    normal\n\
style parentnode     default/default bold\n\
style node           default/default normal\n\
style bullet         cyan/default    normal\n\
style selected       yellow/red      bold\n\
style parentselected yellow/red      bold\n\
style background     default/default normal\n\
\n\
context main\n\
helptext \"ESC| menu || |F2| save || |F3| text || |F5| commandline || |return| edit |\"\n\
bind f1 'status \"This is the minimal configuration, loaded if you did not do make install.\"'\n\
bind .. 'status \"Delete this ~/.tinesrc and replace it with the one in the doc directory.\"'\n\
\n\
bind esc menu\n\
bind ^Q 'context quitsave'\n\
\n\
bind f3 'toggle_text'\n\
bind f5 'commandline'\n\
bind .. 'status --clear'\n\
\n\
bind f6 'macro m_spellcheck'\n\
macro define m_spellcheck\n\
context spellcheck\n\
save_state\n\
spell\n\
end\n\
\n\
bind ^E 'context export'\n\
bind ^L 'context import'\n\
bind ^F 'macro m_search'\n\
macro define m_search\n\
context search\n\
save_state\n\
getquery \"Search for:\"\n\
search -f \"$query\"\n\
end\n\
\n\
bind f9  menu\n\
bind f10 'context user'\n\
\n\
bind ^G 'context grab'\n\
bind .. 'save_state'\n\
\n\
bind ^A 'action'\n\
bind up    up\n\
bind down  down\n\
bind left  left\n\
bind right right\n\
bind npage pagedown\n\
bind ppage pageup\n\
bind home  \"go root\"\n\
\n\
bind return edit\n\
bind del    remove\n\
\n\
bind ins    insert_below\n\
bind .. edit\n\
\n\
bind ^O     insert_below\n\
bind .. edit\n\
\n\
# temporary kludge until we implement insert_above\n\
bind ^B insert_below\n\
bind .. \"movenode up\"\n\
bind .. edit\n\
\n\
bind tab complete\n\
bind ^T  toggle_todo\n\
bind ^D  toggle_done\n\
bind >   indent\n\
bind <   outdent\n\
bind f2  save\n\
\n\
bind ^X save_state\n\
bind .. cut\n\
\n\
bind ^C copy\n\
\n\
bind ^V paste\n\
bind .. down\n\
\n\
bind ^S save_state\n\
bind .. sort\n\
\n\
bind backspace	backspace\n\
bind backspace2	backspace\n\
\n\
bind sleft \"movenode left\"\n\
bind sright	 \"movenode right\"\n\
bind sprevious \"movenode up\"\n\
bind snext \"movenode down\"\n\
\n\
bind + expand\n\
bind - collapse\n\
\n\
bind ^Z stop\n\
\n\
context nodeedit\n\
helptext \"editing node |  |ESC| revert || |return| confirm\"\n\
bind up    up\n\
bind down  down\n\
bind left  left\n\
bind right right\n\
\n\
bind return  confirm\n\
bind esc     cancel\n\
\n\
bind ^S  split\n\
bind ^J  join\n\
\n\
bind backspace  backspace\n\
bind backspace2 backspace\n\
\n\
bind ^A   bol\n\
bind home bol\n\
bind ^E   eol\n\
bind end  eol\n\
\n\
bind ^W   skipword\n\
bind ^B   bskipword\n\
\n\
bind del  delete\n\
\n\
context lineedit\n\
helptext lineedit\n\
\n\
bind up      up\n\
bind down    down\n\
bind left    left\n\
bind right   right\n\
\n\
bind return  confirm\n\
bind esc     cancel\n\
bind tab     complete\n\
\n\
bind backspace backspace\n\
bind backspace2 backspace\n\
bind del        delete\n\
\n\
bind ^A   bol\n\
bind home bol\n\
bind ^E   eol\n\
bind end  eol\n\
\n\
context confirm\n\
helptext \" y | | j | |return| confirm || |any key| cancel\"\n\
\n\
bind y      confirm\n\
bind j      confirm\n\
bind return confirm\n\
\n\
bind any    cancel\n\
\n\
context quitsave\n\
helptext \"Save on exit? |    | Y |es | J |a || | N |o || |ESC| cancel\"\n\
bind Y  save\n\
bind .. quit\n\
\n\
bind y  save\n\
bind .. quit\n\
\n\
bind J  save\n\
bind .. quit\n\
\n\
bind j  save\n\
bind .. quit\n\
\n\
bind N  quit\n\
bind n  quit\n\
\n\
bind esc \"context main\"\n\
bind any unbound\n\
\n\
context spellcheck\n\
helptext \"Spellchecking |   |Space| next node |ESC| cancel |F6,Return| confirm changes\"\n\
\n\
bind space \"go recurse\"\n\
bind ..    spell\n\
\n\
bind down \"go recurse\"\n\
bind ..   spell\n\
\n\
bind up   \"go backrecurse\"\n\
bind ..   spell\n\
\n\
bind return \"context main\"\n\
bind f6 \"context main\"\n\
\n\
bind esc restore_state\n\
bind .. \"context main\"\n\
\n\
bind any unbound\n\
\n\
context grab\n\
helptext \"Node grabbed |  |return| |space| drop here || |ESC| cancel\"\n\
bind esc    restore_state\n\
bind ..     \"context main\"\n\
bind return \"context main\"\n\
bind space  \"context main\"\n\
bind left   \"movenode left\"\n\
bind right  \"movenode right\"\n\
bind up     \"movenode up\"\n\
bind down   \"movenode down\"\n\
bind any    unbound\n\
\n\
context export\n\
helptext \"Export |  | A |scii || | H |tml  html| C |ss  | P |ostscript  | O |PML  |ESC| cancel \"\n\
macro define m_export_ascii\n\
save_state\n\
go root\n\
getquery 'What file should I save the ASCII output in?'\n\
export_ascii $query\n\
restore_state\n\
context main\n\
end\n\
macro define m_export_html\n\
save_state\n\
go root\n\
getquery 'What file should I save the HTML output in?'\n\
export_html $query\n\
context main\n\
restore_state\n\
end\n\
macro define m_export_htmlcss\n\
save_state\n\
go root\n\
getquery 'What file should I save the HTML/CSS output in?'\n\
export_htmlcss $query\n\
context main\n\
restore_state\n\
end\n\
macro define m_export_opml\n\
save_state\n\
go root\n\
getquery 'What file should I save the OPML output in?'\n\
export_opml $query\n\
context main\n\
restore_state\n\
end\n\
macro define m_export_ps\n\
save_state\n\
go root\n\
getquery 'What file should I save the PostScript output in?'\n\
export_ps $query\n\
context main\n\
restore_state\n\
end\n\
bind esc \"context main\"\n\
bind A	\"macro m_export_ascii\"\n\
bind a	\"macro m_export_ascii\"\n\
bind H  \"macro m_export_html\"\n\
bind h  \"macro m_export_html\"\n\
bind C  \"macro m_export_htmlcss\"\n\
bind c  \"macro m_export_htmlcss\"\n\
bind O  \"macro m_export_opml\"\n\
bind o  \"macro m_export_opml\"\n\
bind P  \"macro m_export_ps\"\n\
bind p  \"macro m_export_ps\"		\n\
bind any unbound\n\
context import\n\
helptext \"Import |  | A |scii | O |PML  | H |hnb   |ESC| cancel \"\n\
\n\
bind esc \"context main\"\n\
bind A ..\n\
bind .. 'getquery \"What file should I import ASCII from?\"'\n\
bind .. 'import_ascii \"$query\"'\n\
bind .. 'context main'\n\
bind a ..\n\
bind .. 'getquery \"What file should I import ASCII from?\"'\n\
bind .. 'import_ascii \"$query\"'\n\
bind .. 'context main'\n\
bind O ..\n\
bind .. 'getquery \"What file should I import OPML from?\"'\n\
bind .. 'import_opml \"$query\"'\n\
bind .. 'context main'\n\
bind o ..\n\
bind .. 'getquery \"What file should I import OPML from?\"'\n\
bind .. 'import_opml \"$query\"'\n\
bind .. 'context main'\n\
bind H ..\n\
bind .. 'getquery \"What file should I import HNB data from?\"'\n\
bind .. 'import_hnb \"$query\"'\n\
bind .. 'context main'\n\
bind h ..\n\
bind .. 'getquery \"What file should I import HNB data from?\"'\n\
bind .. 'import_hnb \"$query\"'\n\
bind .. 'context main'\n\
\n\
bind any unbound\n\
\n\
context search\n\
helptext \"Searching |  |ESC| cancel || | n |ext || | p |revious || |space|/|return| go\"\n\
bind down   'search -f \"$query\"'\n\
bind up     'search -b \"$query\"'\n\
bind P      'search -b \"$query\"'\n\
bind p      'search -b \"$query\"'\n\
bind n      'search -f \"$query\"'\n\
bind N      'search -f \"$query\"'\n\
\n\
bind return \"context main\"\n\
\n\
bind esc    \"context main\"\n\
bind ..     restore_state\n\
\n\
bind any    unbound\n\
\n\
context user\n\
helptext \"Usermenu | |ESC| cancel |F1| ispell language |F2| browser command |F3| expand all  |F4|  collapse all\"\n\
bind esc \"context main\"\n\
\n\
bind f1  \"context user_ispell\"\n\
bind f2  \"context user_browser\"\n\
bind f3  \"expand --all\"\n\
bind ..  \"context main\"\n\
\n\
bind f4  \"collapse --all\"\n\
bind ..  \"context main\"\n\
\n\
bind any \"unbound\"\n\
\n\
context user_ispell\n\
helptext \"Language for ispell: | |F1| Br.english  |F2| Am.english  |F3| Norwegian bokmål  |F4| Norwegian nynorsk\"\n\
bind f1  'spell_command \"ispell -x -damerican\"'\n\
bind ..  'context main'\n\
\n\
bind f2  'spell_command \"ispell -x -dbritish\"'\n\
bind ..  'context main'\n\
\n\
bind f3  'spell_command \"ispell -x -dbokmål\"'\n\
bind ..  'context main'\n\
\n\
bind f4  'spell_command \"ispell -x -dnynorsk\"'\n\
bind ..  'context main'\n\
\n\
bind esc 'context main'\n\
bind any unbound\n\
\n\
context user_browser\n\
helptext \"Browser action: | |F1| w3m replace hnb  |F2| w3m in rxvt  |F3| Opera, new tab  |F4| Mozilla new window\"\n\
bind f1 'browser_command \"w3m *\"'\n\
bind .. 'context main'\n\
\n\
bind f2 'web_commend \"xterm -T \\\"w3m spawned from hnb\\\" -fn fixed -rv +sb -e w3m * > /dev/null 2>&1 &\"'\n\
bind .. 'context main'\n\
\n\
bind f3 'web_command \"opera -remote 'openURL(*,new-window)' > /dev/null 2>&1 &\"'\n\
bind .. 'context main'\n\
\n\
bind f4 'web_command \"mozilla -remote 'openURL(*,new-window)'	> /dev/null 2>&1 &\"'\n\
bind .. 'context main'\n\
\n\
bind esc 'context main'\n\
bind any unbound\n\
\n\
\n\
##### file\n\
menu \"File\" \"------------\" \"\"\n\
menu \"File\" \"Save      F2\" \"save\"\n\
menu \"File\" \"Save As...  \" \"macro saveas\"\n\
macro define saveas\n\
getquery 'File to save:'\n\
save_state\n\
go root\n\
export_$format $query\n\
restore_state\n\
end\n\
menu \"File\" \"Save Branch \" \"macro savebranch\"\n\
macro define savebranch\n\
getquery 'File to save:'\n\
save_state\n\
export_$format $query\n\
restore_state\n\
end\n\
menu \"File\" \"Revert      \" \"revert\"		\n\
menu \"File\" \"------------\" \"\"\n\
menu \"File\" \"Export    ^E\" \"context export\"\n\
menu \"File\" \"Import    ^L\" \"context import\"\n\
menu \"File\" \"------------\" \"\"\n\
menu \"File\" \"Spell Chk F6\" \"macro m_spellcheck\"\n\
menu \"File\" \"Statistics  \"	\"macro stats\"\n\
macro define stats\n\
status \"File: \\\"$db_file\\\" Format: $format\"\n\
mem\n\
stats			\n\
end\n\
menu \"File\" \"------------\" \"\"\n\
menu \"File\" \"Exit      ^Q\" \"context quitsave\"\n\
##### edit\n\
menu \"Edit\" \"---------\" \"\"\n\
menu \"Edit\" \"Copy   ^C\" \"copy\"\n\
menu \"Edit\" \"Cut    ^X\" \"cut\"\n\
menu \"Edit\" \"Paste  ^V\" \"paste\"\n\
menu \"Edit\" \"---------\" \"\"\n\
menu \"Edit\" \"Search ^F\" \"macro m_search\"\n\
##### entry\n\
menu \"Entry\" \"----------------\" \"\"\n\
menu \"Entry\" \"Edit      return\" \"edit\"\n\
menu \"Entry\" \"Delete       del\" \"remove\"\n\
menu \"Entry\" \"----------------\" \"\"\n\
menu \"Entry\" \"Do Action     ^A\" \"action\"\n\
menu \"Entry\" \"Indent         >\" \"indent\"\n\
menu \"Entry\" \"Outdent        <\" \"outdent\"\n\
menu \"Entry\" \"Toggle Todo   ^T\" \"toggle_todo\"\n\
menu \"Entry\" \"Toggle Done   ^D\" \"toggle_done\"\n\
menu \"Entry\" \"Toggle Text   F3\" \"toggle_text\"\n\
menu \"Entry\" \"----------------\" \"\"\n\
menu \"Entry\" \"Expand         +\" \"expand\"\n\
menu \"Entry\" \"Collapse       -\" \"collapse\"\n\
menu \"Entry\" \"Expand Subtree  \" \"expand --subtree\"\n\
menu \"Entry\" \"Collapse Subtree\" \"collapse --subtree\"\n\
menu \"Entry\" \"Expand All      \"	\"expand --all\"\n\
menu \"Entry\" \"Collapse All    \" \"collapse --all\"\n\
##### Planner\n\
# Planner macros\n\
macro define pl_new_act\n\
add \"Next Actions: \"\n\
go bottom\n\
add \"Action...\"\n\
go down\n\
movenode right\n\
toggle_todo\n\
end\n\
macro define pl_new_proj\n\
add \"Project: \"\n\
go bottom\n\
toggle_todo\n\
add \"Objective: \"\n\
go down\n\
movenode right\n\
add \"Actions...\"\n\
go down\n\
toggle_todo\n\
end\n\
macro define pl_new_wait\n\
add \"Waiting For: \"\n\
go bottom\n\
add \"Items...\"\n\
go down\n\
movenode right\n\
toggle_todo\n\
end\n\
macro define pl_add_cal\n\
getquery \"Month (1-12)\"\n\
temp_str $query\n\
getquery \"Year (4 digits)\"\n\
insert_cal $temp_str $query\n\
end\n\
macro define pl_add_yr\n\
getquery \"Year (4 digits)\"\n\
add \"$query Calendar\"\n\
go bottom\n\
insert_below\n\
movenode right\n\
insert_cal 12 $query\n\
insert_cal 11 $query\n\
insert_cal 10 $query\n\
insert_cal 9 $query\n\
insert_cal 8 $query\n\
insert_cal 7 $query\n\
insert_cal 6 $query\n\
insert_cal 5 $query\n\
insert_cal 4 $query\n\
insert_cal 3 $query\n\
insert_cal 2 $query\n\
insert_cal 1 $query\n\
# needed to remove the blank node at top of cal\n\
go top\n\
remove\n\
end\n\
menu \"Planner\" \"----------------\" \"\"\n\
menu \"Planner\" \"New Actions List\" \"macro pl_new_act\"\n\
menu \"Planner\" \"New Waiting List\" \"macro pl_new_wait\"\n\
menu \"Planner\" \"New Project List\" \"macro pl_new_proj\"\n\
menu \"Planner\" \"Add Month...    \" \"macro pl_add_cal\"\n\
menu \"Planner\" \"Add Year...     \" \"macro pl_add_yr\"\n\
menu \"Planner\" \"----------------\" \"\"\n\
menu \"Planner\" \"Today's Agenda  \" \"today\"\n\
##### level\n\
menu \"Level\" \"------------\" \"\"\n\
menu \"Level\" \"Sort      ^S\" \"sort -a\"\n\
menu \"Level\" \"Sort (desc.)\" \"sort -d\"\n\
menu \"Level\" \"Shuffle     \" \"sort -r\"\n\
\n\
context main\n\
macro define m_about\n\
status \"Tines - an outliner, planner, organizer, and notebook\n\
status \"(c) 2015-2016 Larry Kollar (lkollar@gmail.com)\n\
end\n\
\n\
macro m_about\n\
# vim: tabstop=4 shiftwidth=4\n\
"
