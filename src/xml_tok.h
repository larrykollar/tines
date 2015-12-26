#ifndef XML_TOK_H

/* xml/html tokenizer (c) Øyvind Kolås 2002 */

#define inbufsize 4096
#define outbufsize 4096

enum {
	t_none,
	t_whitespace,
	t_prolog,
	t_dtd,
	t_comment,
	t_word,
	t_tag,
	t_closetag,
	t_closeemptytag,
	t_endtag,
	t_att,
	t_val,
	t_eof,
	t_entity,
	t_error
};


typedef struct {
	FILE *file_in;
	int state;
	char rbuf[outbufsize];
	char curtag[outbufsize];
	int c;
	int c_held;


	unsigned char inbuf[inbufsize];
	int inbuflen;
	int inbufpos;

	int line_no;
} xml_tok_state;

xml_tok_state *xml_tok_init (FILE * file_in);
void xml_tok_cleanup (xml_tok_state * t);

/*	get an xml token
 *	@param data pointer to pointer to string where the resulting data should be put
 * @return token type
 *
 */
int xml_tok_get (xml_tok_state * t, char **data);

/** get a html token
 * same as above, but tries to balance bad html parse
 * trees, tries to do an automatic transformation from
 * html to xhtml
 */
int html_tok_get (xml_tok_state * t, char **data);

#define XML_TOK_H
#endif /*XML_TOK_H */
