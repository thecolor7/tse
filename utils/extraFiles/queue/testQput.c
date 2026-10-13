/* 
 * testQput.c --- tests the qput function for an initialized and uninitialized queue 
 * 
 * Author: Eve H. Belding
 * Created: 10-06-2026
 * Version: 1.0
 * 
 * Description: tries qput on unopened queue then tries qput on opened list
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "queue.h"

int main(void){
	queue_t* testq1 = NULL;
	int ele = 2; //need to do pointer first

	//Fails for unitialized list
	if (qput(testq1, &ele) != 0) {
		printf("Correctly failed for uninitialized queue\n");
  } else {
		printf("Uninitilized queue check failed\n");
		exit(EXIT_FAILURE);
	}

	//Initialize queue
	testq1 = qopen();

	//Succeeds for initialized list
	if (qput(testq1, &ele) == 0) {
		printf("Correctly added for initialized queue\n");
	} else {
		printf("Failed to add to initialized queue\n");
		qclose(testq1); //make sure to close before exiting
		exit(EXIT_FAILURE);
	}

	qclose(testq1);
	exit(EXIT_SUCCESS);
	return(0);
}
