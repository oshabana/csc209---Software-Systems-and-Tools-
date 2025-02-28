#ifndef _PTREE_H_
#define _PTREE_H_
// for pid_t
#include <sys/types.h>

// We are defining the longest path we will test with:
extern const unsigned int MAX_PATH_LENGTH;

/*
 * Data structure for storing information about a single process.
 * Each tree node contains contains a list of children (the next level 
 * of the tree). It also contains a link to a sibling process (to 
 * create the list of children for its parent).
 */
struct TreeNode {
    pid_t pid;
    char *name;
    struct TreeNode *child_procs;     // A list of child processes
    struct TreeNode *next_sibling;    // A link to the next sibling processes.
};
struct child_of{

	pid_t name;
	int num_child;
	char children[121212];

};

/*
 * A PTree is a dynamically allocated tree structure that contains
 * information about the processes in a Unix system. A PTree is represented by
 * a single TreeNode which is the root of the tree.
 */

// Function for generating a PTree given a root process.
struct child_of * generate_struct_children(pid_t pid);
int count_children(struct child_of * root, int * arr);
int generate_ptree(struct TreeNode **root, pid_t pid);
void print_ptree(struct TreeNode *root, int max_depth);

#endif // _PTREE_H_
