extern char token_seperator;

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

char *tokenize (char *haystack);
