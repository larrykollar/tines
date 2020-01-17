/* libcli -- a small commandline interpreter libraray
 * Copyright (C) 2002 �yvind Kol�s
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef CLI_TOKENIZE_H_INCLUDED
#define CLI_TOKENIZE_H_INCLUDED

char **argv_tokenize(char *input_string);
int argc_of_argv(char **argv);

#endif /* CLI_TOKENIZE_H_INCLUDED */
