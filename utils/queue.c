/* 
 * queue.c --- 
 * 
 * Author: Matthew Morales
 * Created: 10-05-2026
 * Version: 1.0
 * 
 * Description: This file is a modeule that implements a modular queue that stores elements of abitrary types or structs
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <stdbool.h>
#include "queue.h"

// Defining structs and types for queue input type modularity

// Defining a node structure which will wrap around an arbitrary typed input element
typedef struct node {
	void *element;
	struct node *next;
} node_t;

// Defining a struct that defines the structure of the queue
typedef struct queue_in {
	node_t *head;
	node_t *tail;
} queue_in_t;

// qopen() Implementation: This function will simply create a queue structure and pass back a queue typed pointer
queue_t* qopen(void) {

	// Allocating memory for the queue structure
	queue_in_t *qp = malloc(sizeof(queue_in_t));

	// Nominal Malloc Allocation Check
	if (qp == NULL) {
		return NULL;
	}

	// Initializing the new empty queue
	qp->head = NULL;
	qp->tail = NULL;

	// Cast back to the generic queue_t* before returning qp
	return (queue_t*)qp;
}

void qclose(queue_t *qp) {
	// Defining qi pointer to hold front and tail pointers (the queue internal structure)
	queue_in_t *qi = (queue_in_t*)qp;

	if (qi == NULL) return;

	// Defining note_t pointers for queue traveral
	node_t *curr_n = qi->head;
	node_t *next_n = NULL;

	// Iterating through the queue to clear out all elements (freeing memory)
	while (curr_n != NULL) {
		// Saving the pointer to the node that this nodes next pointer points too and then freeing the current node and making the new current node the next node for iteration
		next_n = curr_n->next;
		free(curr_n);
		curr_n = next_n;
	}

	// Now with a freed queue, we will free the queues internal structure to completely free the queue
	free(qi);
	return; //eve added
}

int32_t qput(queue_t *qp, void *elementp) {
  // Defining qi pointer to hold front and tail pointers (the queue internal structure)
	queue_in_t *qi = (queue_in_t*)qp;

	// Checking for NULL queue pointer --> Catches a uninitialized queue case
	if (qi == NULL) return 1;

	// Allocating memory for a new node in the queue
	node_t *new_n = malloc(sizeof(node_t));

	// Malloc Check
	if (new_n == NULL) return 1;

	// Initializing the new node
	new_n->element = elementp;
	new_n->next = NULL;

	// Inserting the node at the end of the queue
	if (qi->tail == NULL) {
		// When the queue is empty (qi->tail is NULL when there is no last element present yet meaning the list is empty and head & tail are currently the same)
		qi->head = new_n;
		qi->tail = new_n;
	} else {
		// If the queue is not empty and there is an initialized node at the end of the queue (qi->tail != NULL) then we will append the node to that node
		qi->tail->next = new_n;
		qi->tail = new_n;
	}
	
	return 0;
}

void* qget(queue_t *qp) {
  // Defining qi pointer to hold front and tail pointers (the queue internal structure)
	queue_in_t *qi = (queue_in_t*)qp;

	// Checking if the queue pointer passed into the function is uninitialized/not a valid queue OR if the queue is empty, if either is true, re weill return a NULL pointer
	if (qi == NULL || qi->head == NULL) return NULL;

	// If the queue is initialized and a first element exists then we will continue with passing the pointer to the user and removing the first element of the queue
	// This is achieved with a front_node to save what the current front node is which is used to specifiy the element that will be returned to the user
	node_t *front_n = qi->head;
	void *n_returned = front_n->element;

	// setting the new head of the queue to be the next node specified by the front_n->next pointer leveraging front_n which points to the only front of the queue
	qi->head = front_n->next;
	
	// IMPORTANT: we must check if removing the current node would make the list empty and move the tail pointer to be equal to tail (Both must be NULL)
	if (qi->head == NULL) qi->tail = NULL;

	// We are done, lets now free the internal node
	free(front_n);

	// Now to return the data in the node that the user stored
	return n_returned;
	
}

void qapply(queue_t *qp, void (*fn)(void* elementp)) {
  // Defining qi pointer to hold front and tail pointers (the queue internal structure)
	queue_in_t *qi = (queue_in_t*)qp;

	// Catching the case in which the queue pointer passed in is not valid/uninitialized (=NULL) OR if the function passed in is NULL/is not valid
	if (qi == NULL || fn == NULL) return;

	// setting up for iteration
	node_t *curr_n = qi->head;

	// Now to interate through the nodes in the que
	while (curr_n != NULL) {
		// Execute specified function
		fn(curr_n->element);

		// Increment forward to the next node to continue iteration
		curr_n = curr_n->next;
	}

}

void* qsearch(queue_t *qp, bool (*searchfn)(void* elementp, const void* keyp), const void* skeyp ) {
  // Defining qi pointer to hold front and tail pointers (the queue internal structure)
	queue_in_t *qi = (queue_in_t*)qp;

	// Catching the case in which the queue pointer passed in is not valid/uninitialized (=NULL) OR if the function passed in is NULL/is not valid
	if (qi == NULL || searchfn == NULL) return NULL;
	
	// setting up for iteration
	node_t *curr_n = qi->head;

	// Now to interate through the nodes in the que
	while (curr_n != NULL) {

		// This checks if the searchfn passed into the qsearch function (made specifically for the type that the queue was initialized to hold) and if it returns true then it means the element was found and the pointer to the element (curr_n->element) is returned
		if (searchfn(curr_n->element, skeyp)) return curr_n->element;
		
		// Increment forward to the next node to continue iteration ONLY if the element was not found
		curr_n = curr_n->next;
	}

	// If the entire queue was searched and the node/element of interest was not found then we will return NULL
	return NULL;
}

void* qremove(queue_t *qp, bool(*searchfn)(void* elementp, const void* keyp), const void* skeyp) {
  // Defining qi pointer to hold front and tail pointers (the queue internal structure)
	queue_in_t *qi = (queue_in_t*)qp;

	// Catching the case in which the queue pointer passed in is not valid/uninitialized (=NULL) OR if the function passed in is NULL/is not valid
	if (qi == NULL || searchfn == NULL) return NULL;

	// Now in order to remove with ease we will need a current and previous poiner to easily reconnect the list
	node_t *curr_n = qi->head;
	node_t *prev_n = NULL;

	// Now to iterate through the list searching for the target node/element to be removed from the queue
	while (curr_n != NULL) {
		// Lets first apply and check if the current node is the target node
		if (searchfn(curr_n->element,skeyp)) {

			// MATCH FOUND: now to save the element to return after removal
			void *returned_element = curr_n->element;

			// Checking for the case that the very first element of the list is the target element/node in which case prev_n will be uninitialized and = NULL
			if(prev_n == NULL){
				// Move the head pointer forward an element if we are at the front of the list
				qi->head = curr_n->next;
			} else {
				// Otherwise we are in the middle or end of the list in which prev_n points to the true previous node which we can then make point to the current next bypassing the current node, removing it, and preparing it to be returned
				prev_n->next = curr_n->next;
			}

			// Here we are checking for the case that we removed was at the end of the list in which case we must make the the new tail the prev_n which was already set to equal to curr_n->next which was previously null in the case that this current node was the last in the queue (hopefully that makes sense...)
			if (curr_n == qi->tail) qi->tail = prev_n;

			// Now that we have isolated the target node and returned the queue to it's new updated state we will free the current node to ensure no memory leak
			free(curr_n);

			return returned_element;
		}

		// No match was found since the loop did not evaluate to true so we will move the pointers forward to continue iterating
		prev_n = curr_n;
		curr_n = curr_n->next;
		
	}

	// No element was found after iterating through the entire list, :( , so we return NULL
	return NULL;
}

void qconcat(queue_t *q1p, queue_t *q2p) {
  // Defining qi pointer to hold front and tail pointers (the queue internal structure)
	queue_in_t *q1 = (queue_in_t*)q1p;
	queue_in_t *q2 = (queue_in_t*)q2p;

	// Lets first check that both the pointeres to both queues are valid (!=NULL)
	if (q1 == NULL || q2 == NULL) return;

	// Lets not check if either q1 or a2 are empty lists

	// Checking if q2 is empty, in which case we do nothing, free the q2 pointer and return
	if (q2->head == NULL) {
		free(q2);
		return;
	}

	// Checking if q1 is empty, then q1 takes the head and tail pointer as it's own
	if (q1->head == NULL) {
		q1->head = q2->head;
		q1->tail = q2->tail;
	} else {
		// In the case that both queues are valid queues that contain elements we will simply set the old tail  pointer of q1 to be linked old head pointer of q2 and then set the old tail to be the second queues tail (always assume that q2 is appended to q1)
		q1->tail->next = q2->head;
		q1->tail = q2->tail;
	}

	// now to free the structure (head and tail pointers) of the old q2 to avoid memmory leaks
	free(q2);
	
}
