/* 
 * testQremove.c --- tests the qremove function for unopened list, a value that is in the queue, and a value not in queue
 * 
 * Author: Eve H. Belding
 * Created: 10-07-2026
 * Version: 1.0
 * 
 * Description: Using ints again for easy comparison. I searched for the same value twice to check if the value was
 * removed from the queue while checking that a NULL pointer is returned for a value not found
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "queue.h"

bool searchfn(void* ele, const void* key) {
	if ( (*(int *)ele) == (*(int *)key) ) {
		return true;
  } else {
		return false;
	}
}
void print(void* ele) {
	printf("%d\n", *(int *)ele);
}

int main(void) {
	queue_t *testq = NULL;
	queue_t *removed = NULL;
	int cmp = 2;
	int ele1 = 1;
	int ele2 = 2;
	int ele3 = 3;

	//Test on unopened queue
	if (qremove(testq, searchfn, &cmp) == NULL) {
		printf("Correctly returned null for unopened queue\n");
	} else {
		printf("Failed for unopened queue\n");
		exit(EXIT_FAILURE);
	}
	
	//Initialize queue
	testq = qopen();
	qput(testq, &ele1);
	qput(testq, &ele2);
	qput(testq, &ele3);

	//Used to visually compare
	printf("Before remove:\n");
	qapply(testq, print);

	//Check return value
	if (qremove(testq, searchfn, &cmp) == &ele2) {
		printf("Returned the correct pointer\n");
	} else {
		printf("Failed to return the correct pointer\n");
		qclose(testq);
		exit(EXIT_FAILURE);
	}

	//Visually see removal
	printf("After remove:\n");
	qapply(testq, print);

	/* Search for the same value again - should be NULL now which checks that both returns NULL for
		 not found and that it was actually removed from the queue*/
	removed = qremove(testq, searchfn, &cmp);
  if (removed == NULL) {
    printf("Returned the correct value of NULL for not found\n");
  } else if (removed == &ele2) {
    printf("Did not remove value from queue on first remove\n");
    qclose(testq);
    exit(EXIT_FAILURE);
  } else {
		printf("Did not return NULL for value not in queue\n");
    qclose(testq);
    exit(EXIT_FAILURE);
	}

	qclose(testq);
	exit(EXIT_SUCCESS);
	return(0);
}
 
