/*
 main.c

 CBTree - A concurrent B+Tree based on Lehman & Yao paper
 
 by Ibrahim Umar

 Based on the bpt: B+ Tree implementation
 
 bpt copyright is as below:

 */


/*
 *  bpt.c
 */
#define Version "1.13"
/*
 *
 *  bpt:  B+ Tree Implementation
 *  Copyright (C) 2010  Amittai Aviram  http://www.amittai.com
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *  Author:  Amittai Aviram
 *    http://www.amittai.com
 *    amittai.aviram@gmail.edu or afa13@columbia.edu
 *    Senior Software Engineer
 *    MathWorks, Inc.
 *    3 Apple Hill Drive
 *    Natick, MA 01760
 *  Original Date:  26 June 2010
 *  Last modified: 15 April 2014
 *
 *  This implementation demonstrates the B+ tree data structure
 *  for educational purposes, includin insertion, deletion, search, and display
 *  of the search path, the leaves, or the whole tree.
 *
 *  Must be compiled with a C99-compliant C compiler such as the latest GCC.
 *
 *  Usage:  bpt [order]
 *  where order is an optional argument
 *  (integer MIN_ORDER <= order <= MAX_ORDER)
 *  defined as the maximal number of pointers in any node.
 *
 */

// Uncomment the line below if you are compiling on Windows.
// #define WINDOWS

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>

#include <sys/time.h>
#include "common.h"
#include "locks.h"

#ifdef WINDOWS
#define bool char
#define false 0
#define true 1
#endif


// Default order is 4.
#define DEFAULT_ORDER 336

// Minimum order is necessarily 3.  We set the maximum
// order arbitrarily.  You may change the maximum order.
#define MIN_ORDER 3
#define MAX_ORDER 400

// Constants for printing part or all of the GPL license.
#define LICENSE_FILE "LICENSE.txt"
#define LICENSE_WARRANTEE 0
#define LICENSE_WARRANTEE_START 592
#define LICENSE_WARRANTEE_END 624
#define LICENSE_CONDITIONS 1
#define LICENSE_CONDITIONS_START 70
#define LICENSE_CONDITIONS_END 625


// TYPES.

/* Type representing the record
 * to which a given key refers.
 * In a real B+ tree system, the
 * record would hold data (in a database)
 * or a file (in an operating system)
 * or some other information.
 * Users can rewrite this part of the code
 * to change the type and content
 * of the value field.
 */
/*
typedef struct record {
	int value;
} record;
*/
/* Type representing a node in the B+ tree.
 * This type is general enough to serve for both
 * the leaf and the internal node.
 * The heart of the node is the array
 * of keys and the array of corresponding
 * pointers.  The relation between keys
 * and pointers differs between leaves and
 * internal nodes.  In a leaf, the index
 * of each key equals the index of its corresponding
 * pointer, with a maximum of order - 1 key-pointer
 * pairs.  The last pointer points to the
 * leaf to the right (or NULL in the case
 * of the rightmost leaf).
 * In an internal node, the first pointer
 * refers to lower nodes with keys less than
 * the smallest key in the keys array.  Then,
 * with indices i starting at 0, the pointer
 * at i + 1 points to the subtree with keys
 * greater than or equal to the key in this
 * node at index i.
 * The num_keys field is used to keep
 * track of the number of valid keys.
 * In an internal node, the number of valid
 * pointers is always num_keys + 1.
 * In a leaf, the number of valid pointers
 * to data is always num_keys.  The
 * last leaf pointer points to the next leaf.
 */
/*
typedef struct node {
    
	void ** pointers;
	int * keys;
	struct node * parent;
	bool is_leaf;
	int num_keys;
	struct node * next; // Used for queue.
    
    pthread_spinlock_t lock;
    struct node * right_link;
    int high_key;
    
} node;
*/

// GLOBALS.

/* The order determines the maximum and minimum
 * number of entries (keys and pointers) in any
 * node.  Every node has at most order - 1 keys and
 * at least (roughly speaking) half that number.
 * Every leaf has as many pointers to data as keys,
 * and every internal node has one more pointer
 * to a subtree than the number of keys.
 * This global variable is initialized to the
 * default value.
 */
int order = DEFAULT_ORDER;

/* The queue is used to print the tree in
 * level order, starting from the root
 * printing each entire rank on a separate
 * line, finishing with the leaves.
 */
node * queue = NULL;

/* The user can toggle on and off the "verbose"
 * property, which causes the pointer addresses
 * to be printed out in hexadecimal notation
 * next to their corresponding keys.
 */
bool verbose_output = false;

// GLOBAL LOCK
pthread_spinlock_t global_lock;

// GLOBAL ROOT
//node * root;

// FUNCTION PROTOTYPES.

// Output and utility.

void license_notice( void );
void print_license( int licence_part );
void usage_1( void );
void usage_2( void );
void usage_3( void );
void enqueue( node * new_node );
node * dequeue( void );
int height( node * root );
int path_to_root( node * root, node * child );
void print_leaves( node * root );
void print_tree( node * root );
void find_and_print(node * root, int key, bool verbose);
void find_and_print_range(node * root, int range1, int range2, bool verbose);
int find_range( node * root, int key_start, int key_end, bool verbose,
               int returned_keys[], void * returned_pointers[]);
int cut( int length );

// Insertion.

void * make_record(uintptr_t value);
node * make_node( void );
node * make_leaf( void );
int get_left_index(node * parent, node * left);
node * insert_into_parent(node * root, node * left, int key, node * right);
node * insert_into_new_root(node * left, int key, node * right);
node * start_new_tree(uintptr_t key, void * pointer);
node * insert( node * root, int key, int value );

// Deletion.

int get_neighbor_index( node * n );
node * adjust_root(node * root);
node * coalesce_nodes(node * root, node * n, node * neighbor, int neighbor_index, int k_prime);
node * redistribute_nodes(node * root, node * n, node * neighbor, int neighbor_index,
                          int k_prime_index, int k_prime);
node * delete_entry( node * root, node * n, int key, void * pointer );


/*---------------START PARALEL------------------*/

#define STACK_MAX 100


struct Stack {
    struct node* data[STACK_MAX];
    int         size;
};

typedef struct Stack Stack;


void Stack_Init(Stack *S)
{
    S->size = 0;
}

struct node* Stack_Top(Stack *S)
{
    if (S->size == 0) {
        fprintf(stderr, "Error TOP: stack empty\n");
        exit(-1);
    }
    
    return S->data[S->size-1];
}

void Stack_Push(Stack *S, struct node* d)
{
    if (S->size < STACK_MAX)
        S->data[S->size++] = d;
    else{
        fprintf(stderr, "Error: stack full\n");
        exit(EXIT_FAILURE);
    }
}

struct node* Stack_Pop(Stack *S)
{
    if (S->size == 0){
        fprintf(stderr, "Error POP: stack empty\n");
        exit(EXIT_FAILURE);
    }
    else
        return S->data[--(S->size)];
}

int scannode(uintptr_t key, struct node** temp, int leaf){
    
    int i = 0;
    
    struct node *A = *temp;
    
    while (i < A->num_keys) {
        if (key >= A->keys[i]) i++;
        else break;
    }
    
    /* Follow next_right if high_key is less than searched value*/
    if(A->high_key > 0 && A->high_key <= key){
        *temp = A->right_link;
        return 1;
    }else{
        if(leaf){
            for (i = 0; i < A->num_keys; i++)
                if (A->keys[i] == key) break;
            if (i == A->num_keys)
                *temp = 0;
            else
                *temp = (node *)A->pointers[i];
        }else{
            //if(i == A->num_keys)
            //     *temp = 0;
            //else
                *temp =  (node *)A->pointers[i];
        }
        return 0;
    }
    
}

int search_par(struct node* root, uintptr_t key)
{
    struct node *current = root;
    
    if(root == NULL) return 0;
    
    while (!current->is_leaf) {
        scannode(key, &current, 0);
    }
    
    while ((scannode(key, &current, 1))) {
    
    }
    
    if(current){
        //struct record *rec = (struct record*) current;
        //if (rec->value == key)
            return 1;
    }
    
    return 0;
}

void* get_par(struct node* root, uintptr_t key)
{
    struct node *current = root;
    
    if(root == NULL) return 0;
    
    while (!current->is_leaf) {
        scannode(key, &current, 0);
    }
    
    while ((scannode(key, &current, 1))) {
    
    }
    
    if(current){
        return (void*)current;
    }
    
    return 0;
}

struct node* move_right(uintptr_t key, struct node* t)
{
    struct node* current = t;
    if(current->high_key>0){
    while (scannode(key, &t, 0)) {
        pthread_spin_lock(&t->lock);
        pthread_spin_unlock(&current->lock);
        current = t;
    }
    }
    return current;
}



int insert_par( node ** root, uintptr_t key, void* value ) {
    
	void * pointer;

	node* oldroot;
	
	node *temp = NULL, *current = NULL, *new_leaf = NULL, *old_leaf = NULL, *child = NULL;

    int * temp_keys;
    void ** temp_pointers;
    int insertion_index, split, i, j;

    
	/* Case: the tree does not exist yet.
	 * Start a new tree.
	 */
    
	if (*root == NULL){
		//printf("Try\n");
        //Try lock the global tree
        if(pthread_spin_trylock(&global_lock)==0){
			if(*root == NULL){
				//printf("Proceed\n");
        		pointer = value;
        		*root = start_new_tree(key, pointer);
        		pthread_spin_unlock(&global_lock);
        		return 1;
			}else{
				pthread_spin_unlock(&global_lock);
				//printf("LEaf?%d\n", (*root)->is_leaf);
			}
        }else{
			//printf("Wait\n");
            //Wait here first
            while(global_lock!=0){};
        }
    }
	
    Stack Nstack;
    
    Stack_Init(&Nstack);
    
    current = *root;
	oldroot = current;
    
    while (!current->is_leaf) {
        temp = current;
        if(!scannode(key, &current, 0))
            Stack_Push(&Nstack, temp);
    }
    
    pthread_spin_lock(&current->lock);
    current = move_right(key, current);
    
    //Now check whether the value exists
    for (i = 0; i < current->num_keys; i++){
        if (current->keys[i] == key) {
            pthread_spin_unlock(&current->lock);
            return 0;
        }
	}
    /*
    if (i != current->num_keys){
        pointer = current->pointers[i];
        if (pointer->value == key){
            pthread_spin_unlock(&current->lock);
            return 0;
        }
    }
    */
    pointer = value;
    
    while(1){
        
        if (current->num_keys < order - 1) {
            //insert_into_leaf(current, key, pointer);
            
            if(current->is_leaf){
                insertion_index = 0;
                while (insertion_index < current->num_keys && current->keys[insertion_index] < key)
                    insertion_index++;
                
                for (i = current->num_keys; i > insertion_index; i--) {
                    current->keys[i] = current->keys[i - 1];
                    current->pointers[i] = current->pointers[i - 1];
                }
                current->keys[insertion_index] = key;
                current->pointers[insertion_index] = pointer;
                current->num_keys++;
                
            }else{
                insertion_index = 0;
                
                while (insertion_index <= current->num_keys &&
                       current->pointers[insertion_index] != old_leaf)
                    insertion_index++;
                
                for (i = current->num_keys; i > insertion_index; i--) {
                    current->pointers[i + 1] = current->pointers[i];
                    current->keys[i] = current->keys[i - 1];
                }
                current->pointers[insertion_index + 1] = pointer;
                current->keys[insertion_index] = key;
                current->num_keys++;
            }
             pthread_spin_unlock(&current->lock);
            return 1;
        } else {  // split
            
            if(current->is_leaf){
                
                new_leaf = make_leaf();
                
                temp_keys = malloc( order * sizeof(uintptr_t) );
                if (temp_keys == NULL) {
                    perror("Temporary keys array.");
                    exit(EXIT_FAILURE);
                }
                
                temp_pointers = malloc( order * sizeof(void *) );
                if (temp_pointers == NULL) {
                    perror("Temporary pointers array.");
                    exit(EXIT_FAILURE);
                }
                
                insertion_index = 0;
                while (insertion_index < order - 1 && current->keys[insertion_index] < key)
                    insertion_index++;
                
                for (i = 0, j = 0; i < current->num_keys; i++, j++) {
                    if (j == insertion_index) j++;
                    temp_keys[j] = current->keys[i];
                    temp_pointers[j] = current->pointers[i];
                }
                
                temp_keys[insertion_index] = key;
                temp_pointers[insertion_index] = pointer;
                
                current->num_keys = 0;
                
                split = cut(order - 1);
                
                for (i = 0; i < split; i++) {
                    current->pointers[i] = temp_pointers[i];
                    current->keys[i] = temp_keys[i];
                    current->num_keys++;
                }
                
                for (i = split, j = 0; i < order; i++, j++) {
                    new_leaf->pointers[j] = temp_pointers[i];
                    new_leaf->keys[j] = temp_keys[i];
                    new_leaf->num_keys++;
                }
                
                free(temp_pointers);
                free(temp_keys);
                
                new_leaf->pointers[order - 1] = current->pointers[order - 1];
                current->pointers[order - 1] = new_leaf;
                
                for (i = current->num_keys; i < order - 1; i++)
                    current->pointers[i] = NULL;
                for (i = new_leaf->num_keys; i < order - 1; i++)
                    new_leaf->pointers[i] = NULL;
                
                new_leaf->parent = current->parent;
                
                /* High Keys */
                new_leaf->high_key = current->high_key;
                current->high_key = (new_leaf->keys[0]);
                new_leaf->right_link = current->right_link;
                current->right_link = new_leaf;
                
                old_leaf = current;
                
                pointer = (struct record*) new_leaf;
                key = new_leaf->keys[0];
            }else{
                /* First create a temporary set of keys and pointers
                 * to hold everything in order, including
                 * the new key and pointer, inserted in their
                 * correct places.
                 * Then create a new node and copy half of the
                 * keys and pointers to the old node and
                 * the other half to the new.
                 */
                
                temp_pointers = malloc( (order + 1) * sizeof(node *) );
                if (temp_pointers == NULL) {
                    perror("Temporary pointers array for splitting nodes.");
                    exit(EXIT_FAILURE);
                }
                temp_keys = malloc( order * sizeof(uintptr_t) );
                if (temp_keys == NULL) {
                    perror("Temporary keys array for splitting nodes.");
                    exit(EXIT_FAILURE);
                }
                
                insertion_index = 0;
                
                while (insertion_index <= current->num_keys &&
                       current->pointers[insertion_index] != old_leaf)
                    insertion_index++;
                
                for (i = 0, j = 0; i < current->num_keys + 1; i++, j++) {
                    if (j == insertion_index  + 1) j++;
                    temp_pointers[j] = current->pointers[i];
                }
                
                for (i = 0, j = 0; i < current->num_keys; i++, j++) {
                    if (j == insertion_index ) j++;
                    temp_keys[j] = current->keys[i];
                }
                
                temp_pointers[insertion_index  + 1] = pointer;
                temp_keys[insertion_index ] = key;
                
                /* Create the new node and copy
                 * half the keys and pointers to the
                 * old and half to the new.
                 */
                split = cut(order);
                new_leaf = make_node();
                current->num_keys = 0;
                for (i = 0; i < split - 1; i++) {
                    current->pointers[i] = temp_pointers[i];
                    current->keys[i] = temp_keys[i];
                    current->num_keys++;
                }
                current->pointers[i] = temp_pointers[i];
                key = temp_keys[split - 1];
                for (++i, j = 0; i < order; i++, j++) {
                    new_leaf->pointers[j] = temp_pointers[i];
                    new_leaf->keys[j] = temp_keys[i];
                    new_leaf->num_keys++;
                }
                new_leaf->pointers[j] = temp_pointers[i];
                
                
                
                free(temp_pointers);
                free(temp_keys);
                new_leaf->parent = current->parent;
                for (i = 0; i <= new_leaf->num_keys; i++) {
                    child = new_leaf->pointers[i];
                    child->parent = new_leaf;
                }
                
                /* High Keys & Links */
                new_leaf->high_key = current->high_key;
                current->high_key = (new_leaf->keys[0]);
                new_leaf->right_link = current->right_link;
                current->right_link = new_leaf;
                
                
                /* Insert a new key into the parent of the two
                 * nodes resulting from the split, with
                 * the old node to the left and the new to the right.
                 */
                
                //return insert_into_parent(root, old_node, k_prime, new_node);
                old_leaf = current;
                
                pointer = new_leaf;
                //key = new_leaf->keys[0];
                
                
            }
            //Now we have to create a new root, restrict only 1 thread
            if(Nstack.size == 0){
                if(pthread_spin_trylock(&global_lock)==0){
					if(oldroot == *root){
						*root = make_node();
                		(*root)->keys[0] = key;
                		(*root)->pointers[0] = old_leaf;
                		(*root)->pointers[1] = new_leaf;
                		(*root)->num_keys++;
                		(*root)->parent = NULL;
                		current->parent = *root;
                		new_leaf->parent = *root;
                		pthread_spin_unlock(&old_leaf->lock);
                		pthread_spin_unlock(&global_lock);
                		return 1;
					}else{
						oldroot = *root;
						current = oldroot;
						pthread_spin_unlock(&global_lock);
					}
                }else{
                    //Others wait here first
                    while(global_lock!=0){};
                }
            }else
				current = Stack_Pop(&Nstack);

            pthread_spin_lock(&current->lock);
            
            move_right(key, current);
			
            pthread_spin_unlock(&old_leaf->lock);            

        }
    }
    return 1;
    
}

/* Master deletion function.
 */
int delete_par(node * root, uintptr_t key) {
    
	int j = 0, i = 0, num_pointers;

    struct node* current = NULL;
    void* pointer = NULL;
    
    current = root;
    
    if(current == NULL) return 0;

    while (!current->is_leaf) {
        scannode(key, &current, 0);
    }
    
    pthread_spin_lock(&current->lock);
    current = move_right(key, current);
    
    //Now check whether the value exists
    for (j = 0; j < current->num_keys; j++){
        if (current->keys[j] == key){ //break;
    
    //if (j != current->num_keys){
        pointer = current->pointers[j];
        //if (pointer->value == key){
            // Remove the key and shift other keys accordingly.
            i = 0;
            while (current->keys[i] != key)
                i++;
            for (++i; i < current->num_keys; i++)
                current->keys[i - 1] = current->keys[i];
            
            // Remove the pointer and shift other pointers accordingly.
            // First determine number of pointers.
            num_pointers = current->is_leaf ? current->num_keys : current->num_keys + 1;
            i = 0;
            while (current->pointers[i] != pointer)
                i++;
            for (++i; i < num_pointers; i++)
                current->pointers[i - 1] = current->pointers[i];
            
            
            // One key fewer.
            current->num_keys--;
            
            // Set the other pointers to NULL for tidiness.
            // A leaf uses the last pointer to point to the next leaf.
            if (current->is_leaf)
                for (i = current->num_keys; i < order - 1; i++)
                    current->pointers[i] = NULL;
            else
                for (i = current->num_keys + 1; i < order; i++)
                    current->pointers[i] = NULL;
            pthread_spin_unlock(&current->lock);
            return 1;
        }
    }
    pthread_spin_unlock(&current->lock);
    
	return 0;
}


/*------------------END PARALEL------------------*/



// FUNCTION DEFINITIONS.

// OUTPUT AND UTILITIES

/* Copyright and license notice to user at startup.
 */
void license_notice( void ) {
	printf("bpt version %s -- Copyright (C) 2010  Amittai Aviram "
           "http://www.amittai.com\n", Version);
	printf("This program comes with ABSOLUTELY NO WARRANTY; for details "
           "type `show w'.\n"
           "This is free software, and you are welcome to redistribute it\n"
           "under certain conditions; type `show c' for details.\n\n");
}


/* Routine to print portion of GPL license to stdout.
 */
void print_license( int license_part ) {
	int start, end, line;
	FILE * fp;
	char buffer[0x100];
    
	switch(license_part) {
        case LICENSE_WARRANTEE:
            start = LICENSE_WARRANTEE_START;
            end = LICENSE_WARRANTEE_END;
            break;
        case LICENSE_CONDITIONS:
            start = LICENSE_CONDITIONS_START;
            end = LICENSE_CONDITIONS_END;
            break;
        default:
            return;
	}
    
	fp = fopen(LICENSE_FILE, "r");
	if (fp == NULL) {
		perror("print_license: fopen");
		exit(EXIT_FAILURE);
	}
	for (line = 0; line < start; line++)
		fgets(buffer, sizeof(buffer), fp);
	for ( ; line < end; line++) {
		fgets(buffer, sizeof(buffer), fp);
		printf("%s", buffer);
	}
	fclose(fp);
}


/* First message to the user.
 */
void usage_1( void ) {
	printf("B+ Tree of Order %d.\n", order);
	printf("Following Silberschatz, Korth, Sidarshan, Database Concepts, 5th ed.\n\n");
	printf("To build a B+ tree of a different order, start again and enter the order\n");
	printf("as an integer argument:  bpt <order>  ");
	printf("(%d <= order <= %d).\n", MIN_ORDER, MAX_ORDER);
	printf("To start with input from a file of newline-delimited integers, \n"
           "start again and enter ");
	printf("the order followed by the filename:\n"
           "bpt <order> <inputfile> .\n");
}


/* Second message to the user.
 */
void usage_2( void ) {
	printf("Enter any of the following commands after the prompt > :\n");
	printf("\ti <k>  -- Insert <k> (an integer) as both key and value).\n");
	printf("\tf <k>  -- Find the value under key <k>.\n");
	printf("\tp <k> -- Print the path from the root to key k and its associated value.\n");
	printf("\tr <k1> <k2> -- Print the keys and values found in the range "
           "[<k1>, <k2>\n");
	printf("\td <k>  -- Delete key <k> and its associated value.\n");
	printf("\tx -- Destroy the whole tree.  Start again with an empty tree of the same order.\n");
	printf("\tt -- Print the B+ tree.\n");
	printf("\tl -- Print the keys of the leaves (bottom row of the tree).\n");
	printf("\tv -- Toggle output of pointer addresses (\"verbose\") in tree and leaves.\n");
	printf("\tq -- Quit. (Or use Ctl-D.)\n");
	printf("\t? -- Print this help message.\n");
}


/* Brief usage note.
 */
void usage_3( void ) {
	printf("Usage: ./bpt [<order>]\n");
	printf("\twhere %d <= order <= %d .\n", MIN_ORDER, MAX_ORDER);
}


/* Helper function for printing the
 * tree out.  See print_tree.
 */
void enqueue( node * new_node ) {
	node * c;
	if (queue == NULL) {
		queue = new_node;
		queue->next = NULL;
	}
	else {
		c = queue;
		while(c->next != NULL) {
			c = c->next;
		}
		c->next = new_node;
		new_node->next = NULL;
	}
}


/* Helper function for printing the
 * tree out.  See print_tree.
 */
node * dequeue( void ) {
	node * n = queue;
	queue = queue->next;
	n->next = NULL;
	return n;
}


/* Prints the bottom row of keys
 * of the tree (with their respective
 * pointers, if the verbose_output flag is set.
 */
void print_leaves( node * root ) {
	int i;
	node * c = root;
	if (root == NULL) {
		printf("Empty tree.\n");
		return;
	}
	while (!c->is_leaf)
		c = c->pointers[0];
	while (true) {
		for (i = 0; i < c->num_keys; i++) {
			if (verbose_output)
				printf("%lx ", (unsigned long)c->pointers[i]);
			printf("%lu ", c->keys[i]);
		}
		if (verbose_output)
			printf("%lx ", (unsigned long)c->pointers[order - 1]);
		if (c->pointers[order - 1] != NULL) {
			printf(" | ");
			c = c->pointers[order - 1];
		}
		else
			break;
	}
	printf("\n");
}


/* Utility function to give the height
 * of the tree, which length in number of edges
 * of the path from the root to any leaf.
 */
int height( node * root ) {
	int h = 0;
	node * c = root;
	while (!c->is_leaf) {
		c = c->pointers[0];
		h++;
	}
	return h;
}


/* Utility function to give the length in edges
 * of the path from any node to the root.
 */
int path_to_root( node * root, node * child ) {
	int length = 0;
	node * c = child;
	while (c != root) {
		c = c->parent;
		length++;
	}
	return length;
}


/* Prints the B+ tree in the command
 * line in level (rank) order, with the
 * keys in each node and the '|' symbol
 * to separate nodes.
 * With the verbose_output flag set.
 * the values of the pointers corresponding
 * to the keys also appear next to their respective
 * keys, in hexadecimal notation.
 */
void print_tree( node * root ) {
    
	node * n = NULL;
	int i = 0;
	int rank = 0;
	int new_rank = 0;
    
	if (root == NULL) {
		printf("Empty tree.\n");
		return;
	}
	queue = NULL;
	enqueue(root);
	while( queue != NULL ) {
		n = dequeue();
		if (n->parent != NULL && n == n->parent->pointers[0]) {
			new_rank = path_to_root( root, n );
			if (new_rank != rank) {
				rank = new_rank;
				printf("\n");
			}
		}
		if (verbose_output)
			printf("(%lx)", (unsigned long)n);
		for (i = 0; i < n->num_keys; i++) {
			if (verbose_output)
				printf("%lx ", (unsigned long)n->pointers[i]);
			printf("%lu ", n->keys[i]);
		}
		if (!n->is_leaf)
			for (i = 0; i <= n->num_keys; i++)
				enqueue(n->pointers[i]);
		if (verbose_output) {
			if (n->is_leaf)
				printf("%lx ", (unsigned long)n->pointers[order - 1]);
			else
				printf("%lx ", (unsigned long)n->pointers[n->num_keys]);
		}
		/* Print the high keys here */
        printf("<%lu>", n->high_key);
        printf("| ");
	}
	printf("\n");
}


/* Finds the appropriate place to
 * split a node that is too big into two.
 */
int cut( int length ) {
	if (length % 2 == 0)
		return length/2;
	else
		return length/2 + 1;
}


// INSERTION

/* Creates a new record to hold the value
 * to which a key refers.
 */

void * make_record(uintptr_t value) {
	uintptr_t * new_record = (uintptr_t *) malloc(sizeof(uintptr_t));
	if (new_record == NULL) {
		perror("Record creation.");
		exit(EXIT_FAILURE);
	}
	else {
		*new_record = value;
	}
	return new_record;
}


/* Creates a new general node, which can be adapted
 * to serve as either a leaf or an internal node.
 */
node * make_node( void ) {
	node * new_node;
	new_node = malloc(sizeof(node));
	if (new_node == NULL) {
		perror("Node creation.");
		exit(EXIT_FAILURE);
	}
	new_node->keys = malloc( (order - 1) * sizeof(uintptr_t) );
	if (new_node->keys == NULL) {
		perror("New node keys array.");
		exit(EXIT_FAILURE);
	}
	new_node->pointers = malloc( order * sizeof(void *) );
	if (new_node->pointers == NULL) {
		perror("New node pointers array.");
		exit(EXIT_FAILURE);
	}
	new_node->is_leaf = false;
	new_node->num_keys = 0;
	new_node->parent = NULL;
	new_node->next = NULL;
    new_node->high_key = 0;
    new_node->right_link = NULL;
    
    pthread_spin_init(&new_node->lock, PTHREAD_PROCESS_SHARED);
    
	return new_node;
}

/* Creates a new leaf by creating a node
 * and then adapting it appropriately.
 */
node * make_leaf( void ) {
	node * leaf = make_node();
	leaf->is_leaf = true;
	return leaf;
}


/* Helper function used in insert_into_parent
 * to find the index of the parent's pointer to
 * the node to the left of the key to be inserted.
 */
int get_left_index(node * parent, node * left) {
    
	int left_index = 0;
	while (left_index <= parent->num_keys &&
           parent->pointers[left_index] != left)
		left_index++;
	return left_index;
}





/* Inserts a new key and pointer to a node
 * into a node into which these can fit
 * without violating the B+ tree properties.
 */
node * insert_into_node(node * root, node * n,
                        int left_index, int key, node * right) {
	int i;
    
	for (i = n->num_keys; i > left_index; i--) {
		n->pointers[i + 1] = n->pointers[i];
		n->keys[i] = n->keys[i - 1];
	}
	n->pointers[left_index + 1] = right;
	n->keys[left_index] = key;
	n->num_keys++;
	return root;
}


/* Inserts a new key and pointer to a node
 * into a node, causing the node's size to exceed
 * the order, and causing the node to split into two.
 */
node * insert_into_node_after_splitting(node * root, node * old_node, int left_index,
                                        int key, node * right) {
    
	int i, j, split, k_prime;
	node * new_node, * child;
	int * temp_keys;
	node ** temp_pointers;
    
	/* First create a temporary set of keys and pointers
	 * to hold everything in order, including
	 * the new key and pointer, inserted in their
	 * correct places.
	 * Then create a new node and copy half of the
	 * keys and pointers to the old node and
	 * the other half to the new.
	 */
    
	temp_pointers = malloc( (order + 1) * sizeof(node *) );
	if (temp_pointers == NULL) {
		perror("Temporary pointers array for splitting nodes.");
		exit(EXIT_FAILURE);
	}
	temp_keys = malloc( order * sizeof(uintptr_t) );
	if (temp_keys == NULL) {
		perror("Temporary keys array for splitting nodes.");
		exit(EXIT_FAILURE);
	}
    
	for (i = 0, j = 0; i < old_node->num_keys + 1; i++, j++) {
		if (j == left_index + 1) j++;
		temp_pointers[j] = old_node->pointers[i];
	}
    
	for (i = 0, j = 0; i < old_node->num_keys; i++, j++) {
		if (j == left_index) j++;
		temp_keys[j] = old_node->keys[i];
	}
    
	temp_pointers[left_index + 1] = right;
	temp_keys[left_index] = key;
    
	/* Create the new node and copy
	 * half the keys and pointers to the
	 * old and half to the new.
	 */
	split = cut(order);
	new_node = make_node();
	old_node->num_keys = 0;
	for (i = 0; i < split - 1; i++) {
		old_node->pointers[i] = temp_pointers[i];
		old_node->keys[i] = temp_keys[i];
		old_node->num_keys++;
	}
	old_node->pointers[i] = temp_pointers[i];
	k_prime = temp_keys[split - 1];
	for (++i, j = 0; i < order; i++, j++) {
		new_node->pointers[j] = temp_pointers[i];
		new_node->keys[j] = temp_keys[i];
		new_node->num_keys++;
	}
	new_node->pointers[j] = temp_pointers[i];
	free(temp_pointers);
	free(temp_keys);
	new_node->parent = old_node->parent;
	for (i = 0; i <= new_node->num_keys; i++) {
		child = new_node->pointers[i];
		child->parent = new_node;
	}

    /* High Keys & Links */
    new_node->high_key = old_node->high_key;
    old_node->high_key = (new_node->keys[0]);
    new_node->right_link = old_node->right_link;
    old_node->right_link = new_node;
    
    
	/* Insert a new key into the parent of the two
	 * nodes resulting from the split, with
	 * the old node to the left and the new to the right.
	 */
    
	return insert_into_parent(root, old_node, k_prime, new_node);
}



/* Inserts a new node (leaf or internal node) into the B+ tree.
 * Returns the root of the tree after insertion.
 */
node * insert_into_parent(node * root, node * left, int key, node * right) {
    
	int left_index;
	node * parent;
    
	parent = left->parent;
    
	/* Case: new root. NOTE: GLOBAL LOCK!!!*/
    
	if (parent == NULL)
		return insert_into_new_root(left, key, right);
    //else
    //    pthread_spin_lock(&parent->lock);
    
	/* Case: leaf or node. (Remainder of
	 * function body.)
	 */
    
	/* Find the parent's pointer to the left
	 * node.
	 */
    
	left_index = get_left_index(parent, left);
    
    
	/* Simple case: the new key fits into the node.
	 */
    
	if (parent->num_keys < order - 1){
		struct node * temp = insert_into_node(root, parent, left_index, key, right);
        pthread_spin_unlock(&left->lock);
        pthread_spin_unlock(&right->lock);
        pthread_spin_unlock(&parent->lock);
        return temp;
    }
    
	/* Harder case:  split a node in order
	 * to preserve the B+ tree properties.
	 */
    
	return insert_into_node_after_splitting(root, parent, left_index, key, right);
}


/* Creates a new root for two subtrees
 * and inserts the appropriate key into
 * the new root.
 */
node * insert_into_new_root(node * left, int key, node * right) {
    
	node * root = make_node();
	root->keys[0] = key;
	root->pointers[0] = left;
	root->pointers[1] = right;
	root->num_keys++;
	root->parent = NULL;
	left->parent = root;
	right->parent = root;
    
    //This is unlock for new parent
    pthread_spin_unlock(&left->lock);
    pthread_spin_unlock(&right->lock);
    
	return root;
}



/* First insertion:
 * start a new tree.
 */
node * start_new_tree(uintptr_t key, void * pointer) {
    
	node * root = make_leaf();
	root->keys[0] = key;
	root->pointers[0] = pointer;
	root->pointers[order - 1] = NULL;
	root->parent = NULL;
	root->num_keys++;
    
	return root;
}



// DELETION.

/* Utility function for deletion.  Retrieves
 * the index of a node's nearest neighbor (sibling)
 * to the left if one exists.  If not (the node
 * is the leftmost child), returns -1 to signify
 * this special case.
 */
int get_neighbor_index( node * n ) {
    
	int i;
    
	/* Return the index of the key to the left
	 * of the pointer in the parent pointing
	 * to n.
	 * If n is the leftmost child, this means
	 * return -1.
	 */
	for (i = 0; i <= n->parent->num_keys; i++)
		if (n->parent->pointers[i] == n)
			return i - 1;
    
	// Error state.
	printf("Search for nonexistent pointer to node in parent.\n");
	printf("Node:  %#lx\n", (unsigned long)n);
	exit(EXIT_FAILURE);
}


node * remove_entry_from_node(node * n, uintptr_t key, node * pointer) {
    
	int i, num_pointers;
    
	// Remove the key and shift other keys accordingly.
	i = 0;
	while (n->keys[i] != key)
		i++;
	for (++i; i < n->num_keys; i++)
		n->keys[i - 1] = n->keys[i];
    
	// Remove the pointer and shift other pointers accordingly.
	// First determine number of pointers.
	num_pointers = n->is_leaf ? n->num_keys : n->num_keys + 1;
	i = 0;
	while (n->pointers[i] != pointer)
		i++;
	for (++i; i < num_pointers; i++)
		n->pointers[i - 1] = n->pointers[i];
    
    
	// One key fewer.
	n->num_keys--;
    
	// Set the other pointers to NULL for tidiness.
	// A leaf uses the last pointer to point to the next leaf.
	if (n->is_leaf)
		for (i = n->num_keys; i < order - 1; i++)
			n->pointers[i] = NULL;
	else
		for (i = n->num_keys + 1; i < order; i++)
			n->pointers[i] = NULL;
    
	return n;
}


node * adjust_root(node * root) {
    
	node * new_root;
    
	/* Case: nonempty root.
	 * Key and pointer have already been deleted,
	 * so nothing to be done.
	 */
    
	if (root->num_keys > 0)
		return root;
    
	/* Case: empty root.
	 */
    
	// If it has a child, promote
	// the first (only) child
	// as the new root.
    
	if (!root->is_leaf) {
		new_root = root->pointers[0];
		new_root->parent = NULL;
	}
    
	// If it is a leaf (has no children),
	// then the whole tree is empty.
    
	else
		new_root = NULL;
    
	free(root->keys);
	free(root->pointers);
	free(root);
    
	return new_root;
}


/* Coalesces a node that has become
 * too small after deletion
 * with a neighboring node that
 * can accept the additional entries
 * without exceeding the maximum.
 */
node * coalesce_nodes(node * root, node * n, node * neighbor, int neighbor_index, int k_prime) {
    
	int i, j, neighbor_insertion_index, n_end;
	node * tmp;
    
	/* Swap neighbor with node if node is on the
	 * extreme left and neighbor is to its right.
	 */
    
	if (neighbor_index == -1) {
		tmp = n;
		n = neighbor;
		neighbor = tmp;
	}
    
	/* Starting point in the neighbor for copying
	 * keys and pointers from n.
	 * Recall that n and neighbor have swapped places
	 * in the special case of n being a leftmost child.
	 */
    
	neighbor_insertion_index = neighbor->num_keys;
    
	/* Case:  nonleaf node.
	 * Append k_prime and the following pointer.
	 * Append all pointers and keys from the neighbor.
	 */
    
	if (!n->is_leaf) {
        
		/* Append k_prime.
		 */
        
		neighbor->keys[neighbor_insertion_index] = k_prime;
		neighbor->num_keys++;
        
        
		n_end = n->num_keys;
        
		for (i = neighbor_insertion_index + 1, j = 0; j < n_end; i++, j++) {
			neighbor->keys[i] = n->keys[j];
			neighbor->pointers[i] = n->pointers[j];
			neighbor->num_keys++;
			n->num_keys--;
		}
        
		/* The number of pointers is always
		 * one more than the number of keys.
		 */
        
		neighbor->pointers[i] = n->pointers[j];
        
		/* All children must now point up to the same parent.
		 */
        
		for (i = 0; i < neighbor->num_keys + 1; i++) {
			tmp = (node *)neighbor->pointers[i];
			tmp->parent = neighbor;
		}
	}
    
	/* In a leaf, append the keys and pointers of
	 * n to the neighbor.
	 * Set the neighbor's last pointer to point to
	 * what had been n's right neighbor.
	 */
    
	else {
		for (i = neighbor_insertion_index, j = 0; j < n->num_keys; i++, j++) {
			neighbor->keys[i] = n->keys[j];
			neighbor->pointers[i] = n->pointers[j];
			neighbor->num_keys++;
		}
		neighbor->pointers[order - 1] = n->pointers[order - 1];
	}
    
	root = delete_entry(root, n->parent, k_prime, n);
	free(n->keys);
	free(n->pointers);
	free(n);
	return root;
}


/* Redistributes entries between two nodes when
 * one has become too small after deletion
 * but its neighbor is too big to append the
 * small node's entries without exceeding the
 * maximum
 */
node * redistribute_nodes(node * root, node * n, node * neighbor, int neighbor_index,
                          int k_prime_index, int k_prime) {
    
	int i;
	node * tmp;
    
	/* Case: n has a neighbor to the left.
	 * Pull the neighbor's last key-pointer pair over
	 * from the neighbor's right end to n's left end.
	 */
    
	if (neighbor_index != -1) {
		if (!n->is_leaf)
			n->pointers[n->num_keys + 1] = n->pointers[n->num_keys];
		for (i = n->num_keys; i > 0; i--) {
			n->keys[i] = n->keys[i - 1];
			n->pointers[i] = n->pointers[i - 1];
		}
		if (!n->is_leaf) {
			n->pointers[0] = neighbor->pointers[neighbor->num_keys];
			tmp = (node *)n->pointers[0];
			tmp->parent = n;
			neighbor->pointers[neighbor->num_keys] = NULL;
			n->keys[0] = k_prime;
			n->parent->keys[k_prime_index] = neighbor->keys[neighbor->num_keys - 1];
		}
		else {
			n->pointers[0] = neighbor->pointers[neighbor->num_keys - 1];
			neighbor->pointers[neighbor->num_keys - 1] = NULL;
			n->keys[0] = neighbor->keys[neighbor->num_keys - 1];
			n->parent->keys[k_prime_index] = n->keys[0];
		}
	}
    
	/* Case: n is the leftmost child.
	 * Take a key-pointer pair from the neighbor to the right.
	 * Move the neighbor's leftmost key-pointer pair
	 * to n's rightmost position.
	 */
    
	else {
		if (n->is_leaf) {
			n->keys[n->num_keys] = neighbor->keys[0];
			n->pointers[n->num_keys] = neighbor->pointers[0];
			n->parent->keys[k_prime_index] = neighbor->keys[1];
		}
		else {
			n->keys[n->num_keys] = k_prime;
			n->pointers[n->num_keys + 1] = neighbor->pointers[0];
			tmp = (node *)n->pointers[n->num_keys + 1];
			tmp->parent = n;
			n->parent->keys[k_prime_index] = neighbor->keys[0];
		}
		for (i = 0; i < neighbor->num_keys - 1; i++) {
			neighbor->keys[i] = neighbor->keys[i + 1];
			neighbor->pointers[i] = neighbor->pointers[i + 1];
		}
		if (!n->is_leaf)
			neighbor->pointers[i] = neighbor->pointers[i + 1];
	}
    
	/* n now has one more key and one more pointer;
	 * the neighbor has one fewer of each.
	 */
    
	n->num_keys++;
	neighbor->num_keys--;
    
	return root;
}


/* Deletes an entry from the B+ tree.
 * Removes the record and its key and pointer
 * from the leaf, and then makes all appropriate
 * changes to preserve the B+ tree properties.
 */
node * delete_entry( node * root, node * n, int key, void * pointer ) {
        
	// Remove key and pointer from node.
    
	n = remove_entry_from_node(n, key, pointer);
    
	/* Case:  deletion from the root.
	 */
    /*
	if (n == root)
		return adjust_root(root);
    */
    
	/* Case:  deletion from a node below the root.
	 * (Rest of function body.)
	 */
    
	/* Determine minimum allowable size of node,
	 * to be preserved after deletion.
	 */
    
//	min_keys = n->is_leaf ? cut(order - 1) : cut(order) - 1;
    
	/* Case:  node stays at or above minimum.
	 * (The simple case.)
	 */
    
//	if (n->num_keys >= min_keys)
		return root;
    
//	/* Case:  node falls below minimum.
//	 * Either coalescence or redistribution
//	 * is needed.
//	 */
//    
//	/* Find the appropriate neighbor node with which
//	 * to coalesce.
//	 * Also find the key (k_prime) in the parent
//	 * between the pointer to node n and the pointer
//	 * to the neighbor.
//	 */
//    
//	neighbor_index = get_neighbor_index( n );
//	k_prime_index = neighbor_index == -1 ? 0 : neighbor_index;
//	k_prime = n->parent->keys[k_prime_index];
//	neighbor = neighbor_index == -1 ? n->parent->pointers[1] :
//    n->parent->pointers[neighbor_index];
//    
//	capacity = n->is_leaf ? order : order - 1;
//    
//	/* Coalescence. */
//    
//	if (neighbor->num_keys + n->num_keys < capacity)
//		return coalesce_nodes(root, n, neighbor, neighbor_index, k_prime);
//    
//	/* Redistribution. */
//    
//	else
//		return redistribute_nodes(root, n, neighbor, neighbor_index, k_prime_index, k_prime);
}



/* Master deletion function.

node * delete(node * root, int key) {
    
	node * key_leaf;
	record * key_record;
    
	key_record = find(root, key, false);
	key_leaf = find_leaf(root, key, false);
    
    
	if (key_record != NULL && key_leaf != NULL) {
        pthread_spin_lock(&key_leaf->lock);
        root = delete_entry(root, key_leaf, key, key_record);
		free(key_record);
        pthread_spin_unlock(&key_leaf->lock);
        
	}
    
	return root;
}
*/

void destroy_tree_nodes(node * root) {
	int i;
	if (root->is_leaf)
		for (i = 0; i < root->num_keys; i++)
			free(root->pointers[i]);
	else
		for (i = 0; i < root->num_keys + 1; i++)
			destroy_tree_nodes(root->pointers[i]);
	free(root->pointers);
	free(root->keys);
	free(root);
}


node * destroy_tree(node * root) {
	destroy_tree_nodes(root);
	return NULL;
}

node** cbtree_alloc()
{
	node **root = malloc(sizeof(void*));
	*root = NULL;

	pthread_spin_init(&global_lock, PTHREAD_PROCESS_PRIVATE);
	return root;
}


void initial_add (struct node **root, int num, int range) {
    int i = 0, j = 0;

    while(i < num){
        j = (rand()%range) + 1;
        i += insert_par(root, j, NULL);
    }
}



/*------------------- END BENCHMARK ---------------------*/
#ifndef NOT_STANDALONE

#include "bench.h"

#include<unistd.h>

int main( int argc, char ** argv ) {


struct node *root = NULL;

int myopt = 0;

int s, u, n, i, r, v;       //Various parameters


i = 1023;           //default initial element count
r = 5000000;        //default range size
u = 10;             //default update rate
s = 0;              //default seed
n = 1;              //default number of thread

v = 0;              //default valgrind mode (reduce stats)

fprintf(stderr,"\nConcurrent BTree\n===============\n\n");
if(argc < 2)
fprintf(stderr,"NOTE: No parameters supplied, will continue with defaults\n");
fprintf(stderr,"Use -h switch for help.\n\n");

while( EOF != myopt ) {
    myopt = getopt(argc,argv,"r:n:i:u:s:hb:");
    switch( myopt ) {
            case 'r': r = atoi( optarg ); break;
            case 'n': n = atoi( optarg ); break;
            case 'i': i = atoi( optarg ); break;
            case 'u': u = atoi( optarg ); break;
            case 's': s = atoi( optarg ); break;
            case 'h': fprintf(stderr,"Accepted parameters\n");
            fprintf(stderr,"-r <NUM>    : Range size\n");
            fprintf(stderr,"-u <0..100> : Update ratio. 0 = Only search; 100 = Only updates\n");
            fprintf(stderr,"-i <NUM>    : Initial tree size (inital pre-filled element count)\n");
            fprintf(stderr,"-n <NUM>    : Number of threads\n");
            fprintf(stderr,"-s <NUM>    : Random seed. 0 = using time as seed\n");
            fprintf(stderr,"-h          : This help\n\n");
            fprintf(stderr,"Benchmark output format: \n\"0: range, insert ratio, delete ratio, #threads, attempted insert, attempted delete, attempted search, effective insert, effective delete, effective search, time (in msec)\"\n\n");
            exit(0);
    }
}
fprintf(stderr,"Parameters:\n");
fprintf(stderr,"- Range size r:\t\t %d\n", r);
fprintf(stderr,"- Update rate u:\t %d%% \n", u);
fprintf(stderr,"- Number of threads n:\t %d\n", n);
fprintf(stderr,"- Initial tree size i:\t %d\n", i);
fprintf(stderr,"- Random seed s:\t %d\n", s);

if (s == 0)
srand((int)time(0));
else
srand(s);

    fprintf(stderr, "Node size: %lu bytes\n", sizeof(node) + ((order - 1) * sizeof(int)) + (order * sizeof(void *)) );

    
    pthread_spin_init(&global_lock, PTHREAD_PROCESS_SHARED);

#if !defined(__TEST)

if (i > 0){
    fprintf(stderr,"Now pre-filling %d random elements...\n", i);
    initial_add(&root, i, r);
    fprintf(stderr,"...Done!\n\n");
}

    start_benchmark(&root, r, u, n, v);

#else

	testpar(&root, u, n, 1);
	testseq(&root, 1);
    
#endif

return 0;
}

#endif
