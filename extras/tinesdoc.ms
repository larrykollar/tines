.nr PS 12p
.nr VS 14p
.nr PSINCR 2p
.nr GROWPS 4
.RP no
.TL
Tines Manual
.AU
Larry Kollar
.AB no
This manual was exported from the default Tines tree
using Export->XSLT (see extras/export_xslt.rc)
and slightly cleaned up.
.AE
.NH 1
Introduction to Tines
.LP
Tines is an outliner, a planner, an organizer, and a notebook. It is based on the old hnb (hierarchical notebook) software, originally written and maintained by Oyvind Kolas. Larry Kollar (lkollar@gmail.com) currently maintains Tines.
.LP
You can use Tines as a straight outliner, editing individual files in OPML, hnb (a simple XML format), or tab-indented text formats.
.LP
A more powerful way to use Tines is to keep all your working outlines, project notes, and calendar entries in a single file. If you do not specify a file name when starting Tines, it uses .tines in your home directory.
.NH 2
Installing Tines
.NH 3
Unix
.NH 4
Prepackaged Binaries
.LP
At this point, there are no packages for Tines. Prepackaged binaries for the older hnb exist for some operating-systems/distributions, these might not be the latest versions.
.IP \[bu] 4
Andras Bali maintains a Debian package of hnb, which can be installed by issuing:
.IP
apt-get install hnb
.IP \[bu] 4
Patrick Li maintains a Free-BSD ports package of hnb.
.IP \[bu] 4
A MacOS X install package is planned, although it will likely be behind the latest version.
.NH 4
Compiling from Source
.LP
Tines source is available from github. To get the latest source, use the following command at the shell prompt:
.LP
git clone https://github.com/larrykollar/tines
.LP
Once you have the source, installing it should be simple:
.LP
make
.br
sudo make install
.SH 5
Platform-specific workarounds:
.SH 5
SuSE Linux:
.LP
SuSE Linux doesn't install ncurses as curses, which ncurses actually does by default if it is allowed to. Changing the CFLAGS line in src/Makefile to -lncurses instead of -lcurses makes hnb compile on SuSE.
.SH 5
OS X / Darwin:
.LP
Check Makefile and src/Makefile for OSX-specific commands and options, and make adjustments accordingly.
.LP
The preprocessor included with earlier versions of OS X doesn't work properly. Adding -no-cpp-precomp to the CFLAGS definition in src/Makefile should make it compile properly.
.LP
This does not apply to Snow Leopard or later versions of OSX. It compiles without any problems on Yosemite.
.LP
Note that hnb compiles on 64-bit Macs but segfaults at startup because pointers were being cast as int (this probably applies to all 64-bit systems). Tines does not have this issue.
.NH 2
Starting Tines
.LP
When you start Tines without specifying a file name or other options, it automatically loads the default ~/.tines file. This is the best way to run Tines when using it as a planner.
.LP
If ~/.tines doesn't exist, Tines loads a starter database with basic information, including this manual.
.LP
Press ESC (or F9) to display the Tines menu. You can edit the ~/.tinesrc file to customize the menu.
.LP
The full commandline usage for Tines is:
.LP
tines [options] [filename] [-e 'command' ['command' ['command' ...]]]
.LP
Available options are:
.IP \[bu] 4
-h or --help show recognized options
.IP \[bu] 4
-a or --ascii specifies a tab-indented ASCII file.
.IP
Note: ASCII doesn't recognize or save attributes like to-do status, done etc.
.IP \[bu] 4
-o or --opml specifies an OPML file.
.IP \[bu] 4
-x or --xml specifies a generic XML file. This mode lets you use Tines as a basic XML editor, collapsing all but the portion you are working on.
.IP
Note: Like ASCII, generic XML files do not recognize attributes such as to-do items.
.IP \[bu] 4
--hnb specifies an hnb file. This is the current default; future versions of Tines will use OPML as the default version.
.IP \[bu] 4
--rc=<file> specifies a configuration file. The default is ~/.tinesrc (Tines creates it if it does not exist).
.IP \[bu] 4
--ui=<type> specifies the interface. The choices are: curses (default), cli, or keygrab (a test mode that outputs hex values for each keypress; press 'q' to quit).
.NH 2
Running CLI Commands
.LP
Tines has a CLI mode that exposes a great deal of the internal functionality. This can be useful for scripts. For example:
.LP
tines -e 'go_root' 'export_htmlcss test.html'
.LP
Tines loads the default file, moves to the start (uppermost node), and exports it with the HTML+CSS filter to a file called test.html.
.LP
While running Tines interactively, press F5 to change to CLI mode. This allows you to use advanced commands not exposed by the default menu. Enter a blank line to return to curses mode.
.NH 2
Quitting Tines
.LP
Press ^Q (Ctrl+Q) to quit. Tines asks if you want to save before quitting. Press Y to save, N to quit without saving, or ESC to continue working.
.NH 2
Importing data
.LP
To import the contents of another file:
.IP \[bu] 4
Press ^L (Ctrl+L) or choose File -> Import in the menu.
.IP \[bu] 4
Choose the desired format. You can choose between OPML, hnb, or tab-indented text formats.
.IP \[bu] 4
Enter the relative or full path of the filename you want to import.
.IP \[bu] 4
Tines imports the file contents under the current level.
.NH 2
Exporting data
.LP
To export the current file:
.IP \[bu] 4
Press ^E (Ctrl+E) or choose File -> Export from the menu.
.IP \[bu] 4
Choose the desired format: OPML, HTML, HTML+CSS, hnb, PostScript, or tab-indented text.
.IP \[bu] 4
Enter the relative or full path of the filename you want to write to.
.LP
Tines 1.10 provides a script in the "extras" directory that exports the selected subtree to a temporary file, then processes it using XSLT. This can be used to export an outline to a working format such as Markdown or Groff. See the extras/README file for setup details.
.NH 2
Printing
.LP
Tines does not support direct printing. You can export to PostScript and send that to your printer queue. You can also use HTML or HTML+CSS to print from a browser.
.NH 1
Using Outlines
.NH 2
Managing entries
.NH 3
Navigating
.LP
The arrow keys move among the entries. Up and Down move within the current level. Left moves to the parent entry. Right moves to the first child entries (or creates one).
.LP
To jump to an entry on the current level, enter the first few letters of that entry. Press Tab twice to enter the sub-level under the chosen entry.
.LP
PageUp and PageDown skip a large amount of entries up and down. Practically, they move you to the top or bottom of the current level.
.LP
Home jumps to the root (the first entry) of the tree.
.NH 3
Searching
.LP
To begin a search:
.IP \[bu] 4
Press ^F (Ctrl+F). Tines prompts you for a search term, then moves to the first entry containing the search term.
.IP \[bu] 4
To find the next occurrence of the search term, press Down or N.
.IP \[bu] 4
To find the previous occurrence press Up or P.
.IP \[bu] 4
To exit search mode at the current entry, press space or Return.
.IP \[bu] 4
To exit search mode and return to the node where you began the search, press ESC.
.NH 3
Moving entries
.LP
To move the current entry, hold Shift and press the arrow keys. This may not work on all computers or in UTF-8 terminal windows.
.LP
To 'grab' the current entry, press ^G (Ctrl+G). In Grab mode, use the arrow keys (without a modifier) to move the entry. Press Space or Return to confirm the move, or ESC to return the entry to its previous position.
.LP
To copy an entry, press ^C (Ctrl+C).
.LP
To cut an entry, press ^X (Ctrl+X).
.LP
To paste an entry below the current location, press ^V (Ctrl+V).
.NH 3
Adding entries
.LP
To add a new entry below the current one, press Insert or ^O (Ctrl+O). You will also immediately enter edit mode.
.LP
To add a new entry above the current one, press ^B (Ctrl+B).
.LP
To add a new entry at the bottom of the current level, type a unique string of characters and press Return.
.NH 3
Deleting entries
.LP
To delete the current entry, press Delete. If the entry has children, you will be asked to confirm the deletion.
.LP
To cut the current entry, press ^X (Ctrl+X). Tines does not confirm the action, but the data is available on the clipboard and can be pasted.
.NH 3
Indenting and Outdenting entries
.LP
Press > to move the current entry and all the following entries of the same level to be children of the preceding entry.
.LP
Press < to move the current entry and all the following entries of the same level to follow the parent of the preceding entry.
.NH 3
Expanding and Collapsing entries
.LP
To expand an entry to display its sub-entries, press the '+' key. This is a sticky expansion; Tines continues to display the sub-entry when you move away.
.LP
To hide expanded entries, press the '-' key.
.LP
To expand the entire outline, select File -> Expand All in the menu.
.LP
To collapse the entire outline, select File -> Collapse All in the menu.
.LP
To expand or collapse all entries under the current entry, select Entry -> Expand Subtree or Entry -> Collapse Subtree.
.NH 2
Editing entries
.LP
To edit the current entry, press Return.
.LP
In edit mode, the cursor keys move between the characters of the data in the entry instead among the entries.
.LP
The default editing shortcuts are:
.IP \[bu] 4
^A (Ctrl+A) or Home: moves to the beginning of the entry.
.IP \[bu] 4
^E (Ctrl+E) or End: moves to the end of the entry.
.IP \[bu] 4
^W (Ctrl+W) moves forward one word.
.IP \[bu] 4
^B (Ctrl+B) moves back one word.
.IP \[bu] 4
^S (Ctrl+S) splits the entry at the cursor.
.IP \[bu] 4
^J (Ctrl+J) joins the following entry to the current entry. Has no effect if the following entry has sub-entries.
.IP \[bu] 4
Return exits edit mode, keeping your changes.
.IP \[bu] 4
ESC exits edit mode, canceling your changes.
.NH 2
Spell-checking
.LP
Press F6 to spell-check the current entry. By default, tines uses ispell; you can specify a different spell-checker in ~/.tinesrc.
.LP
When the current entry is done:
.IP \[bu] 4
Press Space to spell check the next entry.
.IP \[bu] 4
Press Return to exit spell-checking and accept the changes.
.IP \[bu] 4
Press ESC to exit spell-checking and cancel changes.
.NH 2
Sorting entries
.LP
Use the Level menu to sort entries at the current level.
.IP \[bu] 4
Sort is a typical alphanumeric sort.
.IP \[bu] 4
Sort (desc.) sorts in reverse order.
.IP \[bu] 4
Shuffle randomizes the entries.
.NH 2
Setting the entry type
.LP
Press F3 to change the entry to a text type. A text entry has a "t" before the bullet. This can be used with XSLT stylesheets to transform an hnb or OPML file to a document with headings, body text, and bulleted lists. Press F3 again to revert the entry to a plain type.
.LP
Press ^T (Ctrl+T) to change the entry to a todo type. See "Planner Functions" below for more information.
.NH 1
Other features
.NH 2
Planner Functions
.LP
Besides its outlining capabilities, Tines has a number of features making it suitable as a planner. The Planner menu provides convenient access to these functions. You can customize and extend the Planner menu by editing ~/.tinesrc.
.NH 3
Todo-list mode
.LP
Press ^T (Ctrl+T) to toggle between a checkbox and a bullet for the current entry. When the entry has a checkbox, press ^D (Ctrl+D) to check or uncheck the box.
.LP
If an entry has a clear (not checked) box, and sub-entries with checkboxes, Tines shows a percentage of checked sub-entries instead of the empty box.
.NH 3
Inserting Calendar Entries
.LP
The Planner menu can insert entries for a specified month, or an entire year. The calendar entries are no different than other entries, besides being automatically inserted.
.LP
To insert a monthly calendar, select Planner -> Add Month... from the menu. You are prompted for the month (1-12) and year (four digits).
.LP
To insert a yearly calendar, select Planner -> Add Year... from the menu. You are prompted for the year (four digits).
.NH 3
Inserting Lists
.LP
The Planner menu also provides pre-formatted Actions, Waiting For, and Project lists. These types of lists are based on the 'Getting Things Done' (GTD) methodology, conceived by David Allen.
.LP
If you prefer a different methodology, you can modify the Planner menu (by editing ~/.tinesrc) as you see fit.
.NH 2
Using URLs
.LP
If an entry contains an URL, press ^A (Ctrl+A) to open that URL in a browser.
.LP
The default browser is lynx. The ~/.tinesrc file has a list of supported browsers; you can select any of them by commenting out the current selected 'web_command' line and uncommenting another.
.NH 2
Using Email addresses
.LP
If an entry contains an email address, press ^A (Ctrl+A) to open the configured mail client.
.LP
The default mail client is mutt. The ~/.tinesrc file has a short list of supported clients; you can select any of them by commenting out the current selected 'mail_command' line and uncommenting another.
.NH 1
Configuring Tines
.LP
When you start Tines for the first time, it creates a file named .tinesrc in your home directory. This file controls the default configuration. All the commandline functions are available for use in the configuration file.
.LP
For safety, make a copy of the configuration file and edit it as you see fit. If your copy is called test.tines, you can load it instead of the default configuration with the following command:
.LP
tines --rc=test.tines
.LP
If you edited your .tinesrc, and caused problems you cannot fix, delete or rename .tinesrc. Next time you start Tines, it wil reload a default configuration.
.LP
You can edit the configuration file in any text editor. The file is tab-indented, so you can use Tines to edit it with the following command:
.LP
tines -a ~/.tinesrc
.LP
Tines collapses each section; you can expand them as needed to focus only on the part you want.
.NH 2
Context
.LP
Contexts provide a convenient way to change keybindings, depending on what you are doing at any given moment. Tines provides four default contexts:
.IP \[bu] 4
main
.IP \[bu] 4
nodeedit
.IP \[bu] 4
lineedit
.IP \[bu] 4
confirm
.LP
You can define up to 12 custom contexts as well. To define or switch to a context, use the following command:
.LP
context \<name>
.NH 2
Macros
.LP
Tines supports macros. Macros are named collections of CLI commands that can be executed as needed. See the 'Commandline Reference' below for a list of Tines CLI commands.
.LP
To define a macro, use the following sequence of commands:
.LP
macro define macro_name
.br
cmd
.br
cmd...
.br
end
.LP
To execute a macro, use 'macro macro_name'
.NH 2
Key Bindings
.LP
Key bindings allow you to assign one or more actions to a keypress in a given context. Valid keys include:
.IP \[bu] 4
Any single character, including punctuation and similar characters. Case is significant; a and A can be bound to two different functions.
.IP \[bu] 4
The sequence '^X' to indicate a control character (Ctrl+X in this example). Note that Ctrl+H is named backspace2, and Ctrl+M is named return.
.IP \[bu] 4
Function keys f1 through f12
.IP \[bu] 4
Arrow keys: up, down, left, right. With Shift modifier: sleft, sright, sup, sdown.
.IP \[bu] 4
Navigation and editing keys: npage (page down), ppage (page up), home, end, return, enter, ins (insert), del (delete), backspace (0x7f), backspace2 (0x08), tab, esc, space
.IP \[bu] 4
The special key 'any' to indicate any unbound key. This can be used to set up a default action. Its usual purpose is to return an error message using 'bind any unbound'.
.LP
To set up a key binding, use the 'bind' command. Arguments are the key and the action. Substitute '..' for the key if you want to bind more actions to a single keypress. For example, the following key binding substitutes for an 'insert_above' command and is used in the default .tinesrc configuration:
.LP
bind ^B insert_below
.br
bind .. "movenode up"
.br
bind .. edit
.LP
Note that you have to enclose a command in quotes if it has an argument.
.NH 2
Helper Commands
.LP
These commands specify external utilities to invoke when the "action" command detects certain patterns in the current entry.
.NH 3
web_command <command>
.LP
Runs the specified command in response to an http:// action. This usually opens a web browser, but could be used with curl or wget to save a copy instead. Use the * character to substitute the URL.
.NH 3
mail_command <command>
.LP
Runs the specified command in response to an "@" action. This usually opens a mail client, but could be used to trigger other actions. Use the * character to substitute the email address.
.NH 3
spell_command <command>
.LP
Runs the specified command in response to the "spell" command. This usually opens a spell checker.
.NH 3
Related commands
.LP
The "system" command opens a shell and runs its argument as its command line. While it has no effect on the current database, it can be used to process an exported file.
.LP
If the "action" command runs on an entry that begins with "exec," Tines runs the rest of the line as a shell command.
.NH 1
Commandline Reference
.LP
Tines includes an extensive set of CLI commands.
.LP
The commands listed are sorted by function and the file name where they are defined.
.LP
To set a variable, use the command \<variable> \<value>.
.NH 2
Actions (actions.c)
.NH 3
system <command_string>
.LP
Opens a shell and runs the specified command.
.NH 3
action
.LP
Executes an external shell according to node content. It first checks the direct children, then the current node.
.LP
If it contains an http:// URL, the command described in browser_command is executed.
.LP
If it contains an '@' character, the characters surrounding it are interpreted as an email address and the mail_command is executed.
.LP
If the data starts with 'exec' the rest of the data is executed verbatim.
.NH 3
web_command <command_string>
.LP
The command executed when opening URLs. The detected URL replaces the * character.
.NH 3
mail_command <command_string>
.LP
The command executed when sending email. The address replaces the * character.
.NH 3
address (string)
.LP
The email address used as the parameter for mail_command.
.NH 2
Autosave (autosave.c)
.NH 3
tree_changed [increment]
.LP
An internal command, used to drive the autosave functionality. For large changes, pass a high number (around 1000) to make sure the changes are saved immediately. Otherwise, omit the increment.
.NH 3
autosave_check_timeout
.LP
Checks the timeout counter and autosaves if necessary.
.NH 3
autosave_timeout (integer)
.LP
The number of ticks before autosaving after change.
.NH 3
autosave_sync (integer)
.LP
A flag that determines whether the filesystem should be synced after autosave.
.NH 3
autosave_timer (integer)
.LP
The number of ticks since an unsaved change.
.NH 3
autosave_threshold (integer)
.LP
Autosave for every autosave_threshold nodes changed.
.NH 3
autosave_threshold_nodes_changed (integer)
.LP
The number of nodes changed since the last save.
.NH 2
Calendar (cal.c)
.NH 3
insert_cal <month> <year>
.LP
Inserts a monthly calendar below the current selection.
.IP \[bu] 4
<month>: 1-12
.IP \[bu] 4
<year>: the full (4-digit) year.
.NH 2
Macros (cli_macro.c)
.NH 3
macro [define] <cmd>
.LP
Defines or executes a macro.
.LP
When defining a macro, the commands following are part of the macro until reaching an 'end' command.
.NH 2
Clipboard (clipboard.c)
.NH 3
copy
.LP
Stores the current node and its sub-entries in the clipboard.
.NH 3
cut
.LP
Moves the current node and its subtree to the clipboard.
.NH 3
paste
.LP
Inserts the contents of the clipboard at the current position in the tree.
.NH 2
Event control (evilloop.c)
.NH 3
quit, q
.LP
Quits tines without prompting to save.
.NH 3
forced_up (integer)
.LP
If true, attempting to move beyond the first sibling moves to the parent entry.
.NH 3
forced_down (integer)
.LP
If true, attempting to move beyond the last sibling moves to the entry below the parent entry.
.NH 3
add_at_top (integer)
.LP
If true, items inserted automatically are added at the top (default 0, at the bottom).
.NH 2
Expand/collapse (expanded.c)
.NH 3
expand [--all|-a|--here|-h|--subtree]
.LP
Expands the current entry to show its sub-entries.
.IP \[bu] 4
Use the -a or --all option to expand all nodes in the tree.
.IP \[bu] 4
Use the -h or --here option to expand all descendents of the current node.
.IP \[bu] 4
Use the --subtree option to recursively expand all descendants of the current entry.
.NH 3
collapse [-a]
.LP
Collapse the current entry's sub-entries.
.IP \[bu] 4
Use the -a option to collapse all entries in the tree.
.IP \[bu] 4
Use the --subtree option to recursively collapse all descendants of the current entry.
.NH 2
File commands (file.c)
.NH 3
save
.LP
Saves the data.
.NH 3
revert
.LP
Reverts to the last saved version.
.NH 2
Import/export text (file_ascii.c)
.NH 3
export_ascii <filename>
.LP
Exports the tree as a tab-indented text file.
.NH 3
import_ascii <filename>
.LP
Imports a tab-indented text file below the current node.
.NH 3
ascii_margin (integer)
.LP
(present but not implemented) The margin that text export wraps at. Set to -1 to disable wrapping.
.NH 2
Import/export binary (file_binary.c)
.LP
These are internal commands used for autosave snapshots.
.NH 3
export_binary <filename>
.LP
Exports the tree as a binary file.
.NH 3
import_binary <filename>
.LP
Imports a binary file below the current node.
.NH 2
Import/export help (file_help.c)
.LP
These commands are used to generate embeddable text files for including into source. These functions will soon be deprecated as Tines will pull files from /usr/local/share in the near future.
.NH 3
export_help <filename>
.LP
Exports to help source format.
.NH 3
import_help <filename>
.LP
Imports help source format into the tree.
.NH 2
Import/export hnb (file_hnb.c)
.NH 3
export_hnb <filename>
.LP
Exports the tree in hnb format.
.NH 3
import_hnb <filename>
.LP
Imports an hnb file below the current node.
.NH 2
Import/export OPML (file_opml.c)
.NH 3
export_opml <filename>
.LP
Exports the tree as OPML.
.NH 3
import_opml <filename>
.LP
Imports an OPML file below the current node.
.NH 2
Import/export XML (file_xml.c)
.NH 3
import_xml <filename>
.LP
Imports an XML file below the current node.
.NH 3
export_xml <filename>
.LP
Exports the current node, its siblings, and all sublevels to 'filename' as XML markup.
.LP
Load an XML file with import_xml or tines -x file.xml to see how it looks inside tines.
.NH 3
xml_cuddle (integer)
.LP
If true, joins the data with nodes if there are no tags within the tag.
.NH 2
Export HTML (file_html.c)
.NH 3
export_html <filename>
.LP
Exports the tree as basic HTML.
.NH 3
export_htmlcss <filename>
.LP
Exports the tree as HTML with embedded CSS.
.NH 2
Export PostScript (file_ps.c)
.NH 3
export_ps <filename>
.LP
Exports the current entry, its siblings, and all sub-entries to a PostScript file suitable for printing.
.NH 2
Marks (mark.c)
.NH 3
set_mark <mark number>
.LP
Defines a location for use with 'go_mark'. The <mark_number> can be 0 through 9.
.NH 3
go_mark <mark_number>
.LP
Moves to a location saved with 'set_mark'.
.NH 2
Attributes (node.c)
.NH 3
att_set <attribute> <value>
.LP
Sets an attribute for the current entry.
.NH 3
att_get <attribute>
.LP
Returns the specified attribute value for the current entry.
.NH 3
att_clear
.LP
Removes the specified attribute.
.NH 3
att_list
.LP
Lists the text and attributes for the current entry.
.NH 2
Prefs (prefs.c)
.NH 3
format (string)
.LP
The format of the current file.
.NH 3
rc_file (string)
.LP
The path and name of the rc file in use (the default is ~/.tinesrc).
.NH 3
db_file (string)
.LP
The path and name of the file tines is using (the default is ~/.tines).
.NH 3
default_db_file (string)
.LP
The path and file name of the default file tines loads if no other file is available on the command line.
.NH 3
showpercent (integer)
.LP
If true, to-do entries with child to-do entries show a percentage of completed sub-entries.
.NH 3
fixedfocus (integer)
.LP
If true, moves the tree up and down around the selection bar, which remains about 1/3 of the way down the screen.
.NH 3
savepos (integer)
.LP
If true, saves the current position along with the data. Supported in both hnb and OPML formats.
.LP
On save, Tines inserts a \<?pos="x"?> processing instruction, where x is the current position. When reading the file, Tines sets the current position to the value found in the processing instruction.
.NH 3
readonly (integer)
.LP
If true, tines does not save any changes.
.NH 3
escdelay (integer)
.LP
Time, in milliseconds, Tines waits before assuming an ESC keypress is meant as an escape rather than part of a command sequence. The default is the value of the curses ESCDELAY variable.
.NH 2
Queries (query.c)
.NH 3
getquery <prompt>
.LP
Inputs a string from the user and puts it into the variable query.
.LP
The search function, and some of the export/import functions can use queries.
.NH 3
query (string)
.LP
The last query. May also be set as a variable.
.NH 2
Search (search.c)
.NH 3
search [-b|-f] <string>
.LP
Searches -b(ackward) or -f(orward) for the next entry with a substring matching the specified string.
.NH 2
Spellcheck (spell.c)
.NH 3
spell [-r]
.LP
Spellchecks the current node, or all children and following siblings recursively (if -r specified).
.LP
This function uses the command defined in 'spell_command'.
.NH 3
spell_command (string)
.LP
The command executed when spell checking a node.
.LP
Tines writes the entries to a temporary file.
.NH 2
Statistics (statcmd.c)
.NH 3
mem
.LP
Reports the memory usage of the application.
.NH 3
stats
.LP
Reports the number of items, leaf items and total number of words in the tree.
.NH 2
Current state (state.c)
.NH 3
save_state
.LP
Saves a temporary copy of the current tree and selected node in memory.
.NH 3
restore_state
.LP
Restores the copy of the tree saved with save_state.
.NH 2
Tree (tree_misc.c)
.NH 3
movenode <up|left|right|down>
.LP
Reposition the current node.
.NH 3
go <up|down|left|right|recurse|backrecurse|root|top|bottom>
.LP
Changes the current position in the tree:
.IP \[bu] 4
up/down: moves up or down in the current level.
.IP \[bu] 4
left: moves to the parent node.
.IP \[bu] 4
right: moves to the first child node. If no child node exists, tines creates one.
.IP \[bu] 4
root: moves to the top of the tree.
.IP \[bu] 4
top/bottom: moves to the first or last node.
.IP \[bu] 4
recurse: moves to the next node in the current level.
.IP \[bu] 4
backrecurse: moves to the first node in the current level.
.NH 3
outdent
.LP
Moves the active item and the following siblings one level to the left.
.NH 3
indent
.LP
Moves the active item and the following siblings one level to the right.
.NH 3
remove
.LP
Removes the active node. If the node has children, you have a chance to confirm the deletion.
.NH 3
commandline
.LP
Invokes the interactive command line in curses mode.
.NH 3
insert_below
.LP
Adds a new entry immediately below the active entry.
.NH 3
sort [-r|-a|-d]
.LP
Sorts the siblings of the currently selected node:
.IP \[bu] 4
-a=ascending
.IP \[bu] 4
-d=descending
.IP \[bu] 4
-r=random (shuffle)
.NH 3
toggle_todo
.LP
Toggles the visibility and usage of the checkbox for the current entry.
.NH 3
toggle_done
.LP
Toggles the checkbox for the currently active entry, if the entry has a checkbox.
.NH 3
toggle_text
.LP
Toggles the text marker for the current entry. (Introduced in 1.9.20.)
.NH 3
bind <key> <action> [parameter]
.LP
Binds the action to the specified key.
.LP
Use the virtual key '..' to bind subsequent actions; this allows binding multiple actions to a single keystroke.
.LP
The virtual key 'any' indicates any unbound keys. This can be used to produce an error message using 'bind any unbound'.
.NH 3
unbound
.LP
Outputs a message, indicating that the pressed key is not bound in the current context.
.NH 3
context <context>
.LP
Changes the current context.
.LP
Predefined contexts include 'main', 'lineedit', 'nodeedit', and 'confirm'. You can define other contexts as needed.
.NH 3
backspace
.LP
Erases the preceding character.
.NH 3
delete
.LP
Erases the following character.
.NH 3
left
.LP
Moves the cursor or selection left.
.NH 3
right
.LP
Moves the cursor or selection right.
.NH 3
up
.LP
Moves the cursor or selection up.
.NH 3
down
.LP
Moves the cursor or selection down.
.NH 3
pagedown
.LP
Moves down one page.
.NH 3
pageup
.LP
Moves up one page.
.NH 3
bkill_word
.LP
Erases the preceding word.
.NH 3
kill_word
.LP
Erases the following word.
.NH 3
bol
.LP
Moves the cursor to the beginning of line.
.NH 3
eol
.LP
Moves to the end of the line.
.NH 3
top
.LP
Moves to the topmost sibling of the current entry.
.NH 3
bottom
.LP
Moves to the bottommost sibling of the current entry.
.NH 3
skipword
.LP
Moves to the next word.
.NH 3
bskipword
.LP
Moves to the previous word.
.NH 3
cancel
.LP
Cancels editing of the node.
.NH 3
complete
.LP
Outputs the possible completions of the entered command or variable.
.NH 3
confirm
.LP
Confirms the question posed, (used in dialogs asking for the users' consent).
.NH 3
ignore
.LP
Ignores a pressed key without giving an error message.
.NH 3
join
.LP
Joins the currently edited node with the following sibling.
.NH 3
split
.LP
Splits the currently edited node at the cursor location.
.NH 3
kill_line
.LP
Erases the currently edited line.
.NH 3
kill_to_bol
.LP
Erases from the cursor to the beginning of the line.
.NH 3
kill_to_eol
.LP
Erases from the cursor to the end of the line.
.NH 2
Bindings (ui_binding.c)
.LP
Editing bindings and contexts.
.NH 3
Configuration commands
.NH 4
bind <key> <action> [parameter]
.LP
Creates a keybinding in the current context. The virtual key .. adds actions to the last-defined key, allowing a single key to perform multiple actions. Each context may have up to 255 key bindings.
.LP
The virtual key 'any' indicates any unbound keys. This can be used to produce an error message using 'bind any unbound'.
.NH 4
unbound
.LP
Displays an error message for keys not bound to another action.
.NH 4
ignore
.LP
Ignores the current input. Can be used to bind a key you want silently ignored.
.NH 4
confirm \<text>
.LP
Confirms the action.
.NH 4
context \<name>
.LP
Switches to the named context. The following contexts are predefined:
.IP \[bu] 4
main: the primary context for navigating through the tree.
.IP \[bu] 4
confirm: used to confirm a user action (such as deleting).
.IP \[bu] 4
nodeedit: used for editing a node.
.IP \[bu] 4
lineedit: not currently used.
.LP
You can define up to 12 custom contexts. The default .tinesrc configuration defines several contexts.
.NH 4
command \<param>
.LP
(executed in evilloop.c) Executes \<param> as a CLI command.
.NH 4
quit
.LP
(executed in evilloop.c) Quits without saving.
.NH 3
Movement commands (entry-level)
.NH 4
up
.LP
Moves the cursor or selection up.
.NH 4
down
.LP
Moves the cursor or selection down.
.NH 4
left
.LP
Moves the cursor or selection left.
.NH 4
right
.LP
Moves the cursor or selection right.
.NH 4
top
.LP
Moves to the topmost sibling of the current entry.
.NH 4
bottom
.LP
Moves to the last sibling of the current entry.
.NH 4
pageup
.LP
Moves up a page of entries.
.NH 4
pagedown
.LP
Moves down a page of entries.
.NH 3
Movement commands (edit-level)
.NH 4
bol
.LP
Moves to the beginning of the entry.
.NH 4
eol
.LP
Moves to the end of the entry.
.NH 4
skipword
.LP
Moves to the beginning of the next word.
.NH 4
bskipword
.LP
Moves the cursor to the beginning of the first word to the left of the cursor.
.NH 3
Editing commands
.NH 4
backspace
.LP
Deletes the character to the left of the cursor.
.NH 4
delete
.LP
Deletes the character under the cursor.
.NH 4
kill_word
.LP
Deletes the word under the cursor.
.NH 4
bkill_word
.LP
Deletes the word to the left of the cursor.
.NH 4
kill_to_bol
.LP
Deletes all the text from the cursor to the beginning of the entry.
.NH 4
kill_to_eol
.LP
Deletes all the text from the cursor to the end of the entry.
.NH 4
kill_line
.LP
Deletes the line under the cursor.
.NH 4
cancel
.LP
Cancels edit mode without saving changes.
.NH 4
complete
.LP
Outputs possible completions of the partially-entered command or variable name.
.NH 4
join
.LP
Combines the subsequent entry with the following entry.
.NH 4
split
.LP
Splits the entry at the cursor position.
.NH 2
CLI Commands (ui_cli.c, libcli/cli.c)
.NH 3
add <string>
.LP
Inserts a new entry at the end of the current level, containing the specified string.
.LP
Note: The position does not move. To operate on the new entry, use 'go bottom' to move to the entry.
.NH 3
ls
.LP
Lists the nodes at the current level.
.NH 3
addc <parent> <string>
.LP
Inserts a new entry under the node named <parent>, with the text specified in <string>.
.NH 3
cd <path>
.LP
Moves to a different part of the tree.
.NH 3
pwd
.LP
Echoes the current path.
.NH 3
show_vars
.LP
Lists all defined variables and their values.
.NH 3
temp_str (string)
.LP
A temporary string variable for use in macros.
.NH 2
Node formatting (ui_draw.c)
.NH 3
display_format <format string>
.LP
Defines how each node is displayed. The format string syntax is:
.IP \[bu] 4
spaces: literal spaces
.IP \[bu] 4
i<width>: indents the specified width in characters
.IP \[bu] 4
-: inserts a bullet
.IP \[bu] 4
d: inserts the actual node data
.IP \[bu] 4
x<width>: reserved
.LP
A typical format string is 'i4- d'.
.NH 3
bullet_leaf <string>
.LP
The format for entries with no sub-entries.
.LP
Example: ' *'
.NH 3
bullet_parent <string>
.LP
The format for entries with unexpanded sub-entries.
.LP
Example: ' +'
.NH 3
bullet_parent_expanded <string>
.LP
The format for entries with expanded sub-entries.
.LP
Example: ' -'
.NH 3
bullet_todo_parent_undone <string>
.LP
The format for unchecked to-do entries with sub-entries.
.LP
Example: '[ ]'
.NH 3
bullet_todo_parent_done <string>
.LP
The format for checked to-do entries with sub-entries.
.LP
Example: '[X]'
.NH 3
bullet_todo_leaf_undone <string>
.LP
The format for unchecked to-do entries with no sub-entries.
.LP
Example: '[ ]'
.NH 3
bullet_todo_leaf_done <string>
.LP
The format for checked to-do entries with no sub-entries.
.LP
Example: '[X]'
.NH 3
bullet_textleaf <string>
.LP
The format for text entries with no sub-entries.
.LP
Example: 't *'
.NH 3
bullet_textparent <string>
.LP
The format for text entries with collapsed sub-entries.
.LP
Example: 't +'
.NH 3
bullet_textparent_expanded <string>
.LP
The format for text entries with expanded sub-entries.
.LP
Example: 't -'
.NH 2
Node editor (ui_edit.c)
.NH 3
edit
.LP
Invokes the node editor for the current node.
.NH 2
Menu definition (ui_menu.c)
.NH 3
menu <menu> <text> <cmd>
.LP
Defines a menu entry.
.IP \[bu] 4
<menu> specifies the menu name.
.IP \[bu] 4
<text> specifies the entry text.
.IP \[bu] 4
<cmd> specifies the associated command, macro, or context.
.NH 2
Messages (ui_overlay.c)
.NH 3
helptext <help for context>
.LP
Defines the help text for the current context.
.LP
The character | alternates between the menuitem and the menutext styles.
.LP
|| is the escape sequence for a single pipe.
.NH 3
status [-c|--clear] <message>
.LP
Adds 'message' as the newest status line.
.LP
Use -c or --clear to clear all pending status messages off the screen.
.LP
BUG: Currently, specifying -c or --clear omits the message.
.NH 3
echo [-c|--clear] <message>
.LP
Alias for status.
.NH 2
Style (ui_style.c)
.NH 3
style <element> <fgcolor/bgcolor> <attributes>
.LP
Changes the drawing style of <element>. Available elements are: menuitem, menutext, parentnode, node, bullet, selected, parentselected and background.
