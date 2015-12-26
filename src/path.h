#ifndef PATH_H
#define PATH_H

/* converts a pathstring to a node in the tree, ps, the tokenizer escapes
 double //'s as a single / without splitting the things at the sides
 a /// means,.. a slash at the end  of token, and new token coming
*/
Node *path2node (char *path, Node *root);


Node *matchpath2node (char *path, Node *root);

/*
 same as above,.. but forcing and recursive (creates the nodes as we go
 if they don't exist)
*/
Node *path2node_make (char *path, Node *root);

/*	create an absolute path like 
	/aaa/bbb/ccc
	for the node (ccc) specified
	
	FIXME: it uses a static char array,.. that is reused..
	probably not the best way.. but..
*/
char *node2path (Node *node);


/* numeric paths...

	/0/3/2/
	means:
		/ goto root
		0 go down none
		/ go right
		3 go down three
		/ go right
		2 go down two
		/ go right
*/
Node *no_path2node (char *path, Node *root);
char *node2no_path (Node *node);

#endif /*PATH_H */
