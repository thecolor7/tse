/* 
 * testLhash.c --- 
 * 
 * Author: Eve H. Belding
 * Created: 11-10-2026
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "lhash.c"
#include "queue.h"
#include "hash.h"

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

void *test1(void *argp) {
	lhash_t *thp;
	void *fail = (void *)(1);
	int len = (int)argp;
	thp = lhopen(len);

	if (thp == NULL) {
		printf("failed to open");
		return(fail);
	}

	student_t *s1 = makeStudent("Eve Belding", 29);
	student_t *s2 = makeStudent("Genevieve Powell", 28);
	//student_t *s3 = makeStudent("Matthew Morales", 27);

	if (lhput(thp, s1, s1->name, strlen(s1->name))){
		printf("failed to put s1");
		return(fail);
	}
	if (lhput(thp, s2, s2->name, strlen(s2->name))){
		printf("failed to put s2");
		return(fail);
	}

	lhclose(thp);
	return((void *)(0));
	
}

int main(void) {
	pthread_t tid1;
	void *arg = (void *)4;

	if (pthread_create(&tid1, NULL, test1, arg) != 0) {
		exit(EXIT_FAILURE);
	}

	if (pthread_join(tid1, NULL) != 0) {
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
