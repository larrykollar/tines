#ifndef FILE_H
#define FILE_H

#if HAVE_LIBXML2==1 && HAVE_LIBXML_PARSER_H==1 && HAVE_LIBXML_TREE_H==1
#define USE_LIBXML
#endif

#define bufsize 4096

/* store node importer state */
typedef struct {
	Node *npos;
	int startlevel;
} import_state_t;

/* initialize importer, tree will be attached under *node */
extern void init_import (import_state_t *is, Node *node);

/* import node into tree */
extern Node *import_node_text (import_state_t * is, int level, char *data);

/* import node into tree */
extern Node *import_node (import_state_t * is, int level, Node *node);


/* glob file names to catch ~/foo */
char* fn_expand( char *s, int globdironly );


int xml_check (char *filename);
int xml_getpos (char *filename);

/* returns 1 if file exists */
int file_check (char *filename);


#endif /* FILE_H */
