/* 
 * lhash.c --- locked hash table 
 * 
 * Author: Eve Belding
 * Created: 11-11-2026
 * Version: 1.0
 * 
 * Description: 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "hash.h"
#include <pthread.h>

typedef struct lhash {
	queue_t *hp;
	pthread_mutex_t m;
} lhash_t;

lhash_t* lhopen(uint32_t hsize) {
	
	//allocating memory for locked queue structure
	lhash_t *lhp = malloc(sizeof(lhash_t));

	//checks if allocation works
	if (lhp == NULL) {
		return NULL;
	}

	//initialize queue
	lhp->hp = hopen(hsize);
	
	//initialize mutex
	pthread_mutex_init(&(lhp->m),NULL);

	return lhp;
}

int32_t lhput(lhash_t *lhp, void *elementp, const char *key, int keylen) {
	//lock mutex
	pthread_mutex_lock(&(lhp->m));

	//put in queue, return 1 if fails
	if (hput(lhp->hp, elementp, key, keylen) == 1) return 1;

	//unlock mutex
	pthread_mutex_unlock(&(lhp->m));
	
	return 0;
}

void lhapply(lhash_t *lhp, void (*fn)(void* elementp)) {
	//lock mutex
	pthread_mutex_lock(&(lhp->m));

	//put in queue
	happly(lhp->hp, fn);

	//unlock mutex
	pthread_mutex_unlock(&(lhp->m));
	return;
}

void* lhsearch(lhash_t *lhp, bool (*searchfn)(void* elementp, const void* keyp), const void* key, int32_t keylen) {
	//lock mutex
	pthread_mutex_lock(&(lhp->m));

	//search in queue
	void *h_searched = hsearch(lhp->hp, searchfn, key, keylen);

	//unlock mutex
	pthread_mutex_unlock(&(lhp->m));

	return h_searched;
}

void lhclose(lhash_t *lhp) {
	//close queue
  hclose(lhp->hp);

	//destroy mutex
	pthread_mutex_destroy(&(lhp->m));

	free(lhp);
	
	return;
}

void *lhremove(lhash_t *lhp, bool (*searchfn)(void* elementp, const void* searchkeyp), const char *key, int32_t keylen) {
	// lock mutex
	pthread_mutex_lock(&(lhp->m));

	//remove specific element from hash
	void *h_removed = hremove(lhp->hp, searchfn, key, keylen);

	//unlock mutex
	pthread_mutex_unlock(&(lhp->m));

	return h_removed;
}

