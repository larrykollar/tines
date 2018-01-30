# Installing Tines

Installing Tines is fairly straightforward.

First, check the Makefile to verify the
destination directories are what you want.

Once you've done that, type `make` and then
`sudo make install` at the shell prompt. It
should be smooth sailing.

## Install Problems

Type 'uname' at the shell
prompt and then 'man install' to make sure the
Makefile is setting the right flags. Send a patch
as an issue or pull request.

## Runtime Problems

Users have reported issues on Arch Linux and
derivatives (see Issue #5 and Issue #6). They
likely stem from a curses mismatch.

If you run across something that doesn't look
right, check the issues. Feel free to add a new
issue if needed (or better yet, send a patch).
