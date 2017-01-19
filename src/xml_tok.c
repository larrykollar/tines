/*
 * xml_tok.c -- pull based xml tokenizer
 *
 * Copyright (C) 2002 Øyvind Kolås <pippin@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59
 * Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "xml_tok.h"

enum {
	s_null = 0,
	s_start,
	s_tag,
	s_tagnamestart,
	s_tagname,
	s_tagnamedone,
	s_intag,
	s_attstart,
	s_attname,
	s_attdone,
	s_att,
	s_atteq,
	s_eqquot,
	s_eqvalstart,
	s_eqapos,
	s_eqaposval,
	s_eqaposvaldone,
	s_eqval,
	s_eqvaldone,
	s_eqquotval,
	s_eqquotvaldone,
	s_tagend,
	s_empty,
	s_emptyend,
	s_whitespace,
	s_whitespacedone,
	s_entitystart,
	s_entity,
	s_entitydone,
	s_word,
	s_worddone,
	s_tagclose,
	s_tagclosenamestart,
	s_tagclosename,
	s_tagclosedone,
	s_tagexcl,
	s_commentdash1,
	s_commentdash2,
	s_incomment,
	s_commentenddash1,
	s_commentenddash2,
	s_commentdone,
	s_dtd,
	s_prolog,
	s_prologq,
	s_prologdone,
	s_eof,
	s_error
};


char *c_ws = " \n\r\t";

enum {
	c_nil = 0,
	c_eat = 1,					/* request that another char be used for the next state */
	c_store = 2					/* store the current char in the output buffer */
};

typedef struct {
	int state;
	char *chars;
	unsigned char r_start;
	unsigned char r_end;
	int next_state;
	int resetbuf;
	int charhandling;
	int return_type;			/* if set return current buf, with type set to the type */
} state_entry;

#define max_entries 20

static state_entry state_table[s_error][max_entries];

static void a (int state, char *chars, unsigned char r_start,
			   unsigned char r_end, int charhandling, int next_state)
{
	int no = 0;

	while (state_table[state][no].state != s_null)
		no++;
	state_table[state][no].state = state;
	state_table[state][no].r_start = r_start;
	if (chars)
		state_table[state][no].chars = strdup (chars);
	state_table[state][no].r_end = r_end;
	state_table[state][no].charhandling = charhandling;
	state_table[state][no].next_state = next_state;
}

static void r (int state, int return_type, int next_state)
{
	state_table[state][0].state = state;
	state_table[state][0].return_type = return_type;
	state_table[state][0].next_state = next_state;
}

/* *INDENT-OFF* */

void init_statetable(void){
	static int inited=0;
	if(inited)return;
	inited=1;
	memset(state_table,0,sizeof(state_table));	
	a(s_start,		"<",	0,0,			c_eat,			s_tag);
	a(s_start,		c_ws,	0,0,			c_eat+c_store,	s_whitespace);
	a(s_start,		"&",	0,0,			c_eat,			s_entitystart);
	a(s_start,		NULL,	0,255,			c_eat+c_store,	s_word);

	a(s_tag,		c_ws,	0,0,			c_eat,			s_tag);
	a(s_tag,		"/",	0,0,			c_eat,			s_tagclose);
	a(s_tag,		"!",	0,0,			c_eat,			s_tagexcl);
	a(s_tag,		"?",	0,0,			c_eat,			s_prolog);
	a(s_tag,		NULL,	0,255,			c_eat+c_store,	s_tagnamestart);

	a(s_tagclose,	NULL,	0,255,			c_eat+c_store,	s_tagclosenamestart);
	a(s_tagclosenamestart,	">",	0,0,	c_eat,			s_tagclosedone);
	a(s_tagclosenamestart,	NULL,	0,255,	c_eat+c_store,	s_tagclosename);
	a(s_tagclosename,	">",	0,0,		c_eat,			s_tagclosedone);
	a(s_tagclosename,	NULL,	0,255,		c_eat+c_store,	s_tagclosename);
	r(s_tagclosedone,	t_closetag,							s_start);

	a(s_whitespace,		c_ws,	0,0,		c_eat+c_store,	s_whitespace);
	a(s_whitespace,		NULL,	0,255,		c_nil,			s_whitespacedone);
	r(s_whitespacedone,	t_whitespace,						s_start);

	a(s_entitystart,";",	0,0,			c_eat,			s_entitydone);
	a(s_entitystart,NULL,	0,255,			c_eat+c_store,	s_entity);
	a(s_entity,		";",	0,0,			c_eat,			s_entitydone);
	a(s_entity,NULL,		0,255,			c_eat+c_store,	s_entity);
	r(s_entitydone,	t_entity,								s_start);

	a(s_word,		c_ws,	0,0,			c_nil,			s_worddone);
	a(s_word,		"<&",	0,0,			c_nil,			s_worddone);
	a(s_word,		NULL,	0,255,			c_eat+c_store,	s_word);
	r(s_worddone,	t_word,									s_start);

	a(s_tagnamestart,c_ws,	0,0,			c_nil,			s_tagnamedone);
	a(s_tagnamestart,	"/>",	0,0,		c_nil,			s_tagnamedone);
	a(s_tagnamestart,NULL,	0,255,			c_eat+c_store,	s_tagname);
	a(s_tagname,	c_ws,	0,0,			c_nil,			s_tagnamedone);
	a(s_tagname,	"/>",	0,0,			c_nil,			s_tagnamedone);
	a(s_tagname,	NULL,	0,255,			c_eat+c_store,	s_tagname);
	r(s_tagnamedone,	t_tag,								s_intag);

	a(s_intag,		c_ws,	0,0,			c_eat,			s_intag);
	a(s_intag,		">",	0,0,			c_eat,			s_tagend);
	a(s_intag,		"/",	0,0,			c_eat,			s_empty);
	a(s_intag,		NULL,	0,255,			c_eat+c_store,	s_attstart);

	a(s_attstart,	c_ws,	0,0,			c_eat,			s_attdone);
	a(s_attstart,	"=/>",	0,0,			c_nil,			s_attdone);
	a(s_attstart,	NULL,	0,255,			c_eat+c_store,	s_attname);
	a(s_attname,	"=/>",	0,0,			c_nil,			s_attdone);
	a(s_attname,	c_ws,	0,0,			c_eat,			s_attdone);
	a(s_attname,	NULL,	0,255,			c_eat+c_store,	s_attname);
	r(s_attdone,	t_att,									s_att);
	a(s_att,		c_ws,	0,0,			c_eat,			s_att);
	a(s_att,		"=",	0,0,			c_eat,			s_atteq);
	a(s_att,		NULL,	0,255,			c_eat,			s_intag);
	a(s_atteq,		"'",	0,0,			c_eat,			s_eqapos);
	a(s_atteq,		"\"",	0,0,			c_eat,			s_eqquot);
	a(s_atteq,		c_ws,	0,0,			c_eat,			s_atteq);
	a(s_atteq,		NULL,	0,255,			c_nil,			s_eqval);

	a(s_eqapos,		"'",	0,0,			c_eat,			s_eqaposvaldone);
	a(s_eqapos,		NULL,	0,255,			c_eat+c_store,	s_eqaposval);
	a(s_eqaposval,		"'",	0,0,		c_eat,			s_eqaposvaldone);
	a(s_eqaposval,		NULL,	0,255,		c_eat+c_store,	s_eqaposval);
	r(s_eqaposvaldone,	t_val,									s_intag);
	
	a(s_eqquot,		"\"",	0,0,			c_eat,			s_eqquotvaldone);
	a(s_eqquot,		NULL,	0,255,			c_eat+c_store,	s_eqquotval);
	a(s_eqquotval,		"\"",	0,0,		c_eat,			s_eqquotvaldone);
	a(s_eqquotval,		NULL,	0,255,		c_eat+c_store,	s_eqquotval);
	r(s_eqquotvaldone,	t_val,									s_intag);

	a(s_eqval,		c_ws,	0,0,			c_nil,			s_eqvaldone);
	a(s_eqval,		"/>",	0,0,			c_nil,			s_eqvaldone);
	a(s_eqval,		NULL,	0,255,			c_eat+c_store,	s_eqval);

	r(s_eqvaldone,	t_val,									s_intag);

	r(s_tagend,		t_endtag,				s_start);

	a(s_empty,		">",0,0,				c_eat,			s_emptyend);
	a(s_empty,		NULL,0,255,				c_eat,			s_empty);
	r(s_emptyend,	t_closeemptytag,						s_start);

	a(s_prolog,		"?",0,0,				c_eat,			s_prologq);
	a(s_prolog,		NULL,0,255,				c_eat+c_store,	s_prolog);
	
	a(s_prologq,	">",0,0,				c_eat,			s_prologdone);
	a(s_prologq,	NULL,0,255,				c_eat+c_store,	s_prolog);
	r(s_prologdone,	t_prolog,				s_start);

	a(s_tagexcl,	"-",0,0,				c_eat,			s_commentdash1);
	a(s_tagexcl,	"D",0,0,				c_nil,			s_dtd);
	a(s_tagexcl,	NULL,0,255,				c_eat,			s_start);

	a(s_commentdash1,	"-",0,0,				c_eat,			s_commentdash2);
	a(s_commentdash1,	NULL,0,255,				c_eat,			s_error);

	a(s_commentdash2,	"-",0,0,				c_eat,			s_commentenddash1);
	a(s_commentdash2,	NULL,0,255,				c_eat+c_store,	s_incomment);

	a(s_incomment   ,	"-",0,0,				c_eat,			s_commentenddash1);
	a(s_incomment   ,	NULL,0,255,				c_eat+c_store,	s_incomment);

	a(s_commentenddash1,	"-",0,0,			c_eat,			s_commentenddash2);
	a(s_commentenddash1,	NULL,0,255,			c_eat+c_store,	s_incomment);

	a(s_commentenddash2,	">",0,0,			c_eat,			s_commentdone);
	a(s_commentenddash2,	NULL,0,255,			c_eat+c_store,	s_incomment);

	r(s_commentdone,	t_comment,				s_start);
	
}

/* *INDENT-ON* */

static int is_oneof (char c, char *chars)
{
	while (*chars) {
		if (c == *chars)
			return 1;
		chars++;
	}
	return 0;
}

static int nextchar (xml_tok_state * t)
{
	int ret;

	if(! (t->inbufpos<t->inbuflen) ){
		t->inbuflen = (int)fread(t->inbuf,1,inbufsize,t->file_in);
		t->inbufpos=0;
		if(!t->inbuflen)
			return -1;
	}
	
	ret=(int) t->inbuf[t->inbufpos++];
	
	if(ret=='\n')
		t->line_no++;
	
	return ret;
}

int xml_tok_get (xml_tok_state * t, char **data)
{
	int rbuflen = 0;
	state_entry *s;

	init_statetable ();
	t->rbuf[rbuflen] = 0;
	while (2 + 2 == 4) {
		if (!t->c_held) {
			t->c = nextchar (t);
			if (t->c == -1)
				return t_eof;
			t->c_held = 1;
		}
		if (t->state == s_dtd) {	/* FIXME: should make better code for skipping DTD */
/*			int angle = 0;*/
			int squote = 0;
			int dquote = 0;
			int abracket = 1;

/*			int sbracket = 0;*/

			t->rbuf[rbuflen++] = t->c;
			t->rbuf[rbuflen] = 0;

			while (abracket) {
				switch (t->c = nextchar (t)) {
					case -1:
						return t_eof;
					case '<':
						if ((!squote) && (!dquote))
							abracket++;
						break;
					case '>':
						if ((!squote) && (!dquote))
							abracket--;
						break;
					case '"':
					case '\'':
					case '[':
					case ']':
					default:
						break;
				}
				t->rbuf[rbuflen++] = t->c;
				t->rbuf[rbuflen] = 0;
			}
			t->c_held = 0;
			t->state = s_start;
			t->rbuf[--rbuflen] = 0;

			return t_dtd;
		} /* end if state is DTD */
        
		s = &state_table[t->state][0];
		while (s->state) {
			if (s->return_type != t_none) {
				*data = t->rbuf;
				t->state = s->next_state;
				if (s->return_type == t_tag)
					strcpy (t->curtag, t->rbuf);
				if (s->return_type == t_endtag)
					*data = t->curtag;
				if (s->return_type == t_closeemptytag)
					*data = t->curtag;
				return s->return_type;
			}
			if ((s->chars && is_oneof (t->c, s->chars)) ||
				((s->r_start + s->r_end)
				 && (t->c >= s->r_start && t->c <= s->r_end))) {
				if (s->charhandling & c_store) {
					t->rbuf[rbuflen++] = t->c;
					t->rbuf[rbuflen] = 0;
				}
				if (s->charhandling & c_eat) {
					t->c_held = 0;
				}
				t->state = s->next_state;
				break;
			}
			s++;
		}
	}
	return t_eof;
}

xml_tok_state *xml_tok_init (FILE * file_in)
{
	xml_tok_state *ret;

	ret = calloc (1, sizeof (xml_tok_state));
	ret->file_in = file_in;
	ret->state = s_start;
	return ret;
}

void xml_tok_cleanup (xml_tok_state * t)
{
	free (t);
}

char *empty_tags[] = {
	"img", "IMG", "br", "BR", "hr", "HR", "META", "meta", "link", "LINK", NULL
};

char *endomission_tags[] = {
	"li", "LI", "p", "P", "td", "TD", "tr", "TR", NULL
};

int string_is_oneof (char *s, char **ss)
{
	while (*ss) {
		if (!strcmp (s, *ss))
			return 1;
		ss++;
	}
	return 0;
}


int html_tok_get (xml_tok_state * s, char **data)
{
	static int got_a_stored_tag = 0;
	static char stored_tag[4096];
	static int stored_type = t_eof;
	static char opentags[64][64];
	static int level = 0;
	char *rdata;
	int type;

	if (got_a_stored_tag) {
		got_a_stored_tag = 0;
		*data = (char *) &stored_tag;
		return stored_type;
	}

	type = xml_tok_get (s, &rdata);

	switch (type) {
		case t_tag:
			if (level >= 0) {
				if (!strcmp (opentags[level - 1], rdata)) {
					got_a_stored_tag = 1;
					strcpy (stored_tag, rdata);
					stored_type = type;
					return t_closetag;
				}
			}
			strcpy (opentags[level], rdata);
			level++;
			break;
		case t_endtag:
			*data = rdata;
			if (string_is_oneof (rdata, empty_tags)) {
				level--;
				return t_closeemptytag;
			}
			break;
		case t_closeemptytag:
		case t_closetag:		/* FIXME: do more than one level */
			level--;
			if (strcmp (opentags[level], rdata)) {
				fprintf (stderr, "%s/%s\n", opentags[level], rdata);

				got_a_stored_tag = 1;
				stored_type = t_closetag;
				strcpy (stored_tag, rdata);
				level--;
				return t_closetag;
			}
			break;
		default:
			break;
	}

	*data = rdata;
	return type;
}
