# tines
Tines is a console-based outliner/planner/notebook.
It is a fork of the hnb outliner,
which has not been updated in >10 years.

The current version, 1.9.20, is based on hnb version 1.9.18pre7
and introduces support for 64-bit computers.
Further updates (so far) include:

* new `--subtree` option to `expand` and `collapse` commands,
merged in from _lhnb_ with other bugfixes and updates.
* new `type="text"` attribute, with supporting command (`toggle_text`)
and string variables.
* Tines now loads `.tinesrc` and `.tines` as default files.
If you have used _hnb_, merge your old data with these new files.

See the ROADMAP file for further planned updates
along the way to version 2.0 and beyond.

The [wiki](https://github.com/larrykollar/tines/wiki) has a
fairly complete and updated set of documentation.
The doc directory provides manpages, sample files,
and general documentation.
