#ifndef TREE_H
#define TREE_H

#include "node.h"

/*flag (attribute) definitions*/
enum {
	F_hidden = 1 << 0,
	F_readonly = 1 << 1,
	F_temp = 1 << 2,
	F_expanded = 1 << 3,
	F_visible = 1 << 12
};

/* creates a new tree and returns a node pointing to it don't store this node
   for reference, keep updating	your pointer into the tree with the pointers 
   returned	from other functions.
   
   Returns: new tree
*/
Node *tree_new ();


/* Given a node in a tree, this function returns a pointer to the root
   of the tree
   
   Returns: tree root   
*/
Node *node_root (Node *node);

/* frees a whole tree from memory, by reference to one of it's nodes
*/
void tree_free (Node *node);

/*returns the topmost of the siblings on the level of node

   Returns: level top
*/
Node *node_top (Node *node);

/*returns the bottommost of the sibling on the lvel of node

   Returns: level bottom
*/
Node *node_bottom (Node *node);

/* inserts a new node above node, returns the new node

   Returns: new node
*/
Node *node_insert_up (Node *node);

/* inserts a new node below node, returns the new node

   Returns: new node
*/
Node *node_insert_down (Node *node);

/*inserts a child for node, if there already is a child, 0 is returned

   Returns: new node
*/
Node *node_insert_right (Node *node);

/* the number of nodes above 

	Returns: number of nodes
*/
unsigned int nodes_up (Node *node);

/* the number of nodes below 

	Returns: number of nodes
*/
unsigned int nodes_down (Node *node);

/* the number of nodes to the left (level of node)

	Returns: number of nodes
*/
unsigned int nodes_left (Node *node);
unsigned int nodes_right (Node *node);

/* removes node and it's children, returns: the 'nearest' still existing node
  (up or down or left of specified node)

	Returns: nearby node
*/
Node *node_remove (Node *node);

/* finds a node starting with match amongst the siblings of where
   returns 0 if no match found or no match string given

	Returns: matching node or NULL
*/
Node *node_match (char *match, Node *where);

/* same as above, but must match whole node

	Returns: matching node or NULL
*/
Node *node_exact_match (char *match, Node *where);

/* returns the next node with a case insensitive substring match from where.

	Returns: matching node or NULL
*/
Node *node_recursive_match (char *match, Node *where);

/* returns the next node with a case insensitive substring match from where.

	Returns: matching node or NULL
*/
Node *node_backrecursive_match (char *match, Node *where);


/* swaps the contents of two nodes
*/
void node_swap (Node *nodeA, Node *nodeB);

/* returns the next node, doing a recursive traversal of the tree 

	Returns: next recursive node or 0 if none
*/
Node *node_recurse (Node *node);

/* returns the previous node, doing a recursive traversal of the tree backwards

	Returns: next back recursive node or 0 if none
*/
Node *node_backrecurse (Node *node);

/* returns the number of this node in the database 
*/
int node_no (Node *node);

/* included from node.h

(most of these are actually macros but can be used as if functions)

   determines if there is a node immedietly next to the specified
   node in given direction, returns the node if there is 0 otherwise

Node *node_up(node);
Node *node_down(node);
Node *node_left(node);
Node *node_right(node);

    sets all the flags of a node, if it exists
	Returns: New flags, or 0 if node didn't exist

int node_setflags(node,flags);

	gets all the flags of a node, if it exists
	Returns: flags, or 0 if node didn't exist
int node_getflags(node);

	returns the state of the specified flag
	Returns: 1 if flag is set 0 if not
int node_getflag(node,flag);

	sets the specified flag if state is 1, turns of the flag if state is 0
	Returns: new flags
int node_setflag(node,flag);

	toggles the specified flag, 
	Returns: 1 if flag were set 0 if flag were turned of
int node_toggleflag(node,flag);

	gets priority of node
	Returns: priority, or 0 if node didn't exist
int node_getpriority(node);

	sets priority of a node, if it exists
	Returns: New priority, or 0 if node didn't exist
int node_setpriority(node,priority);


	sets and gets the data for a node, does neccesary allocating
	and freeing as well.
char *node_setdata(Node *node,char *data);

	Returns: pointer to data
char *node_getdata(Node *node);
*/

Node *tree_duplicate (Node *source, Node *target);

extern char TEXT[5];


#define fixnullstring(str) ((str)?(str):"")


#endif /* TREE_H */
