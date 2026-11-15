/* 
 * testLhash.c --- 
 * 
 * Author: Genevieve Powell
 * Created: 11-11-2026
 * Version: 1.0
 * 
 * Description: currently, this is compiling, reaching success, and has no leaks, but needs to test more functionality 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "lqueue.h"
#include "queue.h"
#include "hash.h"

//necessary test functions
typedef struct student {
	char name[20];
	int year;
} student_t;

student_t *makeStudent(char sname[20], int syear) {
	student_t *s = (student_t *)malloc(sizeof(student_t));
	if (s == NULL) return(NULL);

	strcpy(s->name, sname);
	s->year = syear;

	return(s);
}

void printStudent(void *student) {
	student_t *s = (student_t *)student;
	printf("%s, %d\n", s->name, s->year);
}

bool findStudent(void *element, const void *key) {
	 student_t *s = (student_t *)element;
	 const char *name = (const char *)key;
	 return(strcmp(s->name, name) == 0);
}

//test 1 should print table, find Eve, print again
void *test1(void *argp) {
	lqueue_t *tqp = (lqueue_t *)argp;

	printf("test 1\n");

	lqapply(tqp, printStudent);

	lqsearch(tqp, findStudent, "Eve Belding");

	lqapply(tqp, printStudent);
	
	printf("Success\n");
	return((void *)(0));
	
}

//test 2 should just print
void *test2(void *argp) {
	lqueue_t *tqp = (lqueue_t *)argp;

	printf("test 2\n");
	
	lqapply(tqp, printStudent);
	return((void *)(0));
}

//test 3 also tests lqput - should put in no name and remove genevieve, then print, then remove no name to stop leaks
void *test3(void *argp) {
	lqueue_t *tqp = (lqueue_t *)argp;
	student_t *s = makeStudent("No Name", 30);
	
	printf("test 3\n");
	if (s != NULL) {
		lqput(tqp, s);
	}
	lqget(tqp);
	lqapply(tqp, printStudent);
	if (s != NULL) {
		lqsearch(tqp, findStudent, "No Name");
		free(s);
	}
	
  return((void *)(0));
}

	
int main(void) {
	pthread_t tid1;
	pthread_t tid2;
	pthread_t tid3;

	//Create the base queue
	lqueue_t *queue = lqopen();
	if (queue == NULL) {
		exit(EXIT_FAILURE);
	}

  student_t *s1 = makeStudent("Eve Belding", 29);
  student_t *s2 = makeStudent("Genevieve Powell", 28);
  student_t *s3 = makeStudent("Matthew Morales", 27);
  if (s1 == NULL || s2 == NULL || s3 == NULL) {
		if (s1 != NULL) free(s1);
		if (s2 != NULL) free(s2);
		if (s3 != NULL) free(s3);
		lqclose(queue);
		exit(EXIT_FAILURE);
	}

  if (lqput(queue, s1) || lqput(queue, s2) ||
      lqput(queue, s3)) {
    free(s1);
		free(s2);
		free(s3);
		lqclose(queue);
    exit(EXIT_FAILURE);
  }
	
	void *arg = (void *)queue;

	//Multithreading
	if (pthread_create(&tid1, NULL, test1, arg) != 0) {
		exit(EXIT_FAILURE);
	}
	if (pthread_create(&tid2, NULL, test2, arg) != 0) {
    exit(EXIT_FAILURE);
  }
	if (pthread_create(&tid3, NULL, test3, arg) != 0) {
    exit(EXIT_FAILURE);
  }

	if (pthread_join(tid1, NULL) != 0) {
		exit(EXIT_FAILURE);
	}
	if (pthread_join(tid2, NULL) != 0) {
    exit(EXIT_FAILURE);
  }
	if (pthread_join(tid3, NULL) != 0) {
    exit(EXIT_FAILURE);
  }

	//End by freeing everything
	if (s1 != NULL) free(s1);
  if (s2 != NULL) free(s2);
  if (s3 != NULL) free(s3);
	lqclose(queue);

	exit(EXIT_SUCCESS);
}
