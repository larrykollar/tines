/*	tokenize parses a string into tokens (seperate arguements)
	the global char token_seperator defines what character delimits
	the tokens.
	
	The first call to tokenize should have the input string as 
	the argument. Subsequent calls should have the argument set
	to "", as tokenize will work on a copy of the original string
	and keep record of it's progress.
	
	when tokenize sees two token_seperator chars in a row, it
	will think of this as a single seperator character within a token
	
	tokenize will also return the token_seperator char in a string
	if it is placed first or last in the input string.
	
	(c) Øyvind Kolås 2000
*/

#include <string.h>
#include "tokenizer.h"

#define TOKEN_MAXSIZE 512
char token_seperator = '/';

char *tokenize (char *haystack)
{
	static char empty[2] = "";
	static char token[TOKEN_MAXSIZE];
	static char buffer[TOKEN_MAXSIZE];
	static int pos = 0;
	int tokenpos = 0;

	if (strlen (haystack) >= TOKEN_MAXSIZE)
		return (empty);			/* safety precaution */

	if (haystack[0] != 0) {		/* new tokenization */
		pos = 0;
		token[0] = 0;
		strcpy (buffer, haystack);
	} else {
		if (pos == 0)
			return (empty);		/* we cannot start with an empty string */
	}

	if (pos >= strlen (buffer))
		return (empty);			/* this is the end */

	while (buffer[pos] != 0) {
		if (buffer[pos] == token_seperator) {
			if (buffer[pos + 1] == token_seperator) {	/* escaped seperator char */
				pos++;
			} else {
				pos++;
				if (buffer[pos] == 0) {	/* child of last */
					buffer[pos + 1] = token_seperator;
					buffer[pos + 2] = 0;
					buffer[pos] = token_seperator;
				}

				if ((pos == 1)) {	/* "root" */
					token[0] = token_seperator;
					token[1] = 0;
				}
				return (token);
			}
		}
		token[tokenpos++] = buffer[pos++];
		token[tokenpos] = 0;
	}
	return (token);
}
