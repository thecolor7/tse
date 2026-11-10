/* 
 * lqueue.c --- 
 * 
 * Author: Genevieve M. Powell
 * Created: 11-10-2026
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include "queue.c"
#include <pthread.h>

typedef struct lqueue {
	queue_t *qp;
	pthread_mutex_t m;
} lqueue_t;

lqueue_t* lqopen(void) {
	
	//allocating memory for locked queue structure
	lqueue_t *lqp = malloc(sizeof(lqueue_t));

	//checks if allocation works
	if (lqp == NULL) {
		return NULL;
	}

	//initialize queue
	lqp->qp = qopen();
	
	//initialize mutex
	pthread_mutex_init(&(lqp->m),NULL);

	return lqp;
}

int32_t lqput(lqueue_t *lqp, void *elementp) {
	//lock mutex
	pthread_mutex_lock(&(lqp->m));

	//put in queue, return 1 if fails
	if (qput(lqp->qp, elementp) == 1) return 1;

	//unlock mutex
	pthread_mutex_unlock(&(lqp->m));
	
	return 0;
}

void* lqget(lqueue_t *lqp) {
	//lock mutex
	pthread_mutex_lock(&(lqp->m));

	//put in queue
	void *q_returned = qget(lqp->qp);

	//unlock mutex
	pthread_mutex_unlock(&(lqp->m));

	return q_returned;
}

void lqapply(lqueue_t *lqp, void (*fn)(void* elementp)) {
	//lock mutex
	pthread_mutex_lock(&(lqp->m));

	//put in queue
	qapply(lqp->qp, fn);

	//unlock mutex
	pthread_mutex_unlock(&(lqp->m));
	return;
}

void* lqsearch(lqueue_t *lqp, bool (*searchfn)(void* elementp, const void* keyp), const void* skeyp) {
	//lock mutex
	pthread_mutex_lock(&(lqp->m));

	//search in queue
	void *q_searched = qsearch(lqp->qp, searchfn, skeyp);

	//unlock mutex
	pthread_mutex_unlock(&(lqp->m));

	return q_searched;
}

void lqclose(lqueue_t *lqp) {
	//destroy mutex
	pthread_mutex_destroy(&(lqp->m));

	//close queue
	qclose(lqp->qp);
	return;
}


