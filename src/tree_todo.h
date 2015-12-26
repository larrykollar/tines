#ifndef TREE_TODO_H
#define TREE_TODO_H
#include "node.h"

/* checks/unchecks the todo boxes of parents according to the status of 
   current node and	siblings.
*/
void node_update_parents_todo (Node *pos);

/** @returns an integer between 0 and 1000 according to the 
		completion status of the nodes children
*/
int node_calc_complete (Node *node);

/**
	@returns the accumulated size of the nodes children
*/
int node_calc_size (Node *node);

/*returns the completion status a node should have*/

int done_status (Node *a);

#endif /* TREE_TODO_H */

int calc_percentage_size (Node *node, int *resize);
