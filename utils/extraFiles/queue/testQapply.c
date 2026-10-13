/* 
 * testQapply.c --- tests qapply for an empty queue and a filled queue
 * 
 * Author: Eve H. Belding
 * Created: 10-07-2026
 * Version: 1.0
 * 
 * Description: Test function assumes a queue of ints then doubles and prints each of these elements.
 * Should do nothing for an empty queue, and should print all the values doubled for a full queue.
 * Proves that this will work for any element type and function assming the 2 are compatible
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "queue.h"

void multPrint(void *el) {
	//weird syntax to interpret void as int for test
	int val = *(int *)el;

	//double and print
	val = val*2;
	printf("%d\n", val);

	//make sure new value is placed in each element
	*(int *)el = val;
}

int main(void){
	queue_t *testq = NULL;
	int element1 = 1;
	int element2 = 2;
	int element3 = 3;

	//Test on empty queue - should print nothing and change no elements
	printf("Nothing should print for empty queue\n");
	qapply(testq, multPrint);
	
	//Check logically
  if (element1 == 1 && element2 == 2 && element3 == 3) {
    printf("Correctly did not change any elements\n");
  } else {
    printf("Failed: Altered elements without queue\n");
    qclose(testq);
    exit(EXIT_FAILURE);
  }
	
	//Make initial queue
	testq = qopen();
	qput(testq, &element1);
	qput(testq, &element2);
	qput(testq, &element3);

	//Run test apply (prints within function)
	printf("See function work for full queue (should show 2, 4, 6):\n");
	qapply(testq, multPrint);

	//Check logically
	if (element1 == 2 && element2 == 4 && element3 == 6) {
		printf("Successfully changed all of queue\n");
	} else {
		printf("Failed to change all of queue\n");
		qclose(testq);
		exit(EXIT_FAILURE);
	}
	
	qclose(testq);
	exit(EXIT_SUCCESS);
	return(0);
}
