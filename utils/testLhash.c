/* 
 * testLhash.c --- 
 * 
 * Author: Eve H. Belding
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
#include "lhash.h"
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

//main test function
void *test1(void *argp) {
	void *fail = (void *)(1);
	lhash_t *thp = (lhash_t *)argp;

	printf("test 1\n");

	lhapply(thp, printStudent);

	lhremove(thp, findStudent, "Eve Belding", strlen("Eve Belding"));

	lhapply(thp, printStudent);
	
	printf("Success\n");
	return((void *)(0));
	
}

void *test2(void *argp) {
	lhash_t *thp = (lhash_t *)argp;

	printf("test 2\n");
	
	lhapply(thp, printStudent);
	return((void *)(0));
}

void *test3(void *argp) {
	lhash_t *thp = (lhash_t *)argp;

	printf("test 3\n");
	
	lhremove(thp, findStudent, "Genevieve Powell", strlen("Genevieve Powell"));
	lhapply(thp, printStudent);
	
  return((void *)(0));
}

	
int main(void) {
	pthread_t tid1;
	pthread_t tid2;
	pthread_t tid3;

	//Create different arguments to differ the tests
	lhash_t *hash = lhopen(10);
	if (hash == NULL) {
		exit(EXIT_FAILURE);
	}

  student_t *s1 = makeStudent("Eve Belding", 29);
  student_t *s2 = makeStudent("Genevieve Powell", 28);
  student_t *s3 = makeStudent("Matthew Morales", 27);
  if (s1 == NULL || s2 == NULL || s3 == NULL) {
		if (s1 != NULL) free(s1);
		if (s2 != NULL) free(s2);
		if (s3 != NULL) free(s3);
		lhclose(hash);
		exit(EXIT_FAILURE);
	}

  if (lhput(hash, s1, s1->name, strlen(s1->name)) || lhput(hash, s2, s2->name, strlen(s2->name)) ||
      lhput(hash, s3, s3->name, strlen(s3->name))) {
    free(s1);
		free(s2);
		free(s3);
		lhclose(hash);
    exit(EXIT_FAILURE);
  }
	
	void *arg = (void *)hash;

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

	if (s1 != NULL) free(s1);
  if (s2 != NULL) free(s2);
  if (s3 != NULL) free(s3);
	lhclose(hash);

	exit(EXIT_SUCCESS);
}
