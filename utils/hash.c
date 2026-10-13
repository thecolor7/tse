/* 
 * hash.c -- implements a generic hash table as an indexed set of queues.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "hash.h"
#include "queue.h"

#define get16bits(d) (*((const uint16_t *) (d)))

//define a hashtable element that includes the pointer to the hash and the size of the table
typedef struct hashtable {
	int size;
	queue_t **table;
} hashtable_in_t;

/*
 * SuperFastHash() -- produces a number between 0 and the tablesize-1.
 *
 * The following (rather complicated) code, has been taken from Paul
 * Hsieh's website under the terms of the BSD license. It's a hash
 * function used all over the place nowadays, including Google Sparse
 * Hash.
 */
static uint32_t SuperFastHash (const char *data,int len,uint32_t tablesize) {
  uint32_t hash = len, tmp;
  int rem;
  
  if (len <= 0 || data == NULL)
		return 0;
  rem = len & 3;
  len >>= 2;
  /* Main loop */
  for (;len > 0; len--) {
    hash  += get16bits (data);
    tmp    = (get16bits (data+2) << 11) ^ hash;
    hash   = (hash << 16) ^ tmp;
    data  += 2*sizeof (uint16_t);
    hash  += hash >> 11;
  }
  /* Handle end cases */
  switch (rem) {
  case 3: hash += get16bits (data);
    hash ^= hash << 16;
    hash ^= data[sizeof (uint16_t)] << 18;
    hash += hash >> 11;
    break;
  case 2: hash += get16bits (data);
    hash ^= hash << 11;
    hash += hash >> 17;
    break;
  case 1: hash += *data;
    hash ^= hash << 10;
    hash += hash >> 1;
  }
  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;
  return hash % tablesize;
}

//Opens a new hashtable of size hsize
hashtable_t *hopen(uint32_t hsize) {
	//fail if initial malloc fails
	hashtable_in_t *new = malloc(sizeof(hashtable_in_t));
	if (new == NULL) {
		return(NULL);
	}
	
	new->table = malloc(hsize*sizeof(queue_t *));
	//In case malloc fails, free new and fail
	if (new->table == NULL) {
		free(new);
		return(NULL);
	}
	//need to initilize all queues to NULL to prevent leaks
	for (int i = 0; i<hsize; i++) {
		new->table[i] = NULL;
	}
	//assuming no failures, set size and return the hashtable_t *
	new->size = hsize;			
	return((hashtable_t *)new);
}

//Close a hashtable - important to free everything internal as well
void hclose(hashtable_t *htp) {
	hashtable_in_t *hp = htp;
	
	//if already NULL no need to do anything
	if (hp != NULL) {
		//free each queue
		for (int i = 0; i < hp->size; i++) {
			//if not already NULL, close the queue
			if (hp->table[i] != NULL) {
				qclose(hp->table[i]);
			}
		}
		//free table itself only after - this order is important to prevent leaks
		free(hp->table);
		free(hp);
		htp = NULL;
	}
	return;
}

//puts an element at a specific key in hashtable
int32_t hput(hashtable_t *htp, void *ep, const char *key, int keylen) {
	hashtable_in_t *hp = htp;
	int idx = 0;

	//exit failure if no hp || MATT ADDITION; EXPANDED IF STATEMENT: Added this as an extra safequard to ensure all inputs to the function are properly initialized

	// ORGININAL CODE:
	//if (hp == NULL) {
	//	return(1);
	//}

	// New Addition:
	if (hp == NULL || ep == NULL || key == NULL){
		return(1);
	}
	
	//gets the hash index from key
	idx = SuperFastHash(key, keylen, hp->size);
	//opens new queue if none yet exists
	if (hp->table[idx] == NULL) {
		hp->table[idx] = qopen();
	}
	//puts element in that queue
	qput(hp->table[idx], ep);

	return(0);
}

//applies a function to every element in every queue of hash table
void happly(hashtable_t *htp, void (*fn)(void* ep)) {
	hashtable_in_t *hp = htp;
	//can't do anything if no hp
	if (hp == NULL || fn == NULL) {
    return;
  }

	//loop through all queues, and if queue exists then do function
	for (int i = 0; i < hp->size; i++) {
		if (hp->table[i] != NULL) {
			qapply(hp->table[i], fn);
		}
	}
}

//searches for a given key - first searches the table then searches the queue
void *hsearch(hashtable_t *htp, bool (*searchfn)(void* elementp, const void* searchkeyp),
							const char *key, int32_t keylen) {
	hashtable_in_t *hp = htp;
	void *match = NULL;
	int idx = 0;
	
	//can't do anything if no hp || MATT ADDITION: Expanded if statement as in hput to check for all valid inputs
  if (hp == NULL || searchfn == NULL || key == NULL) {
    return(NULL);
  }

	//Finds the right queue
	idx = SuperFastHash(key, keylen, hp->size);

	//MATT ADDITION: Added a final check to quard against segmentation fault from trying to access a null entry
	if (hp->table[idx] == NULL) {
		return(NULL);
	}

	//Searches within queue
	match = qsearch(hp->table[idx], searchfn, key);
	//qsearch handles making this NULL if not found
	return(match);
}

//finds and removes a certain element - first searches table then queue like search
void *hremove(hashtable_t *htp, bool (*searchfn)(void* elementp, const void* searchkeyp),
							const char *key, int32_t keylen) {
	hashtable_in_t *hp = htp;
  void *match = NULL;
  int idx = 0;

  //can't do anything if no hp  || MATT ADDITION: Expanded if statement as in hput to check for all valid inputs
  if (hp == NULL || searchfn == NULL || key == NULL) {
    return(NULL);
  }

  //Finds the right queue
  idx = SuperFastHash(key, keylen, hp->size);

	//MATT ADDITION: Added a final check to quard against segmentation fault from trying to access a null entry
	if (hp->table[idx] == NULL) {
		return(NULL);
	}
	
  //Searches within queue
  match = qremove(hp->table[idx], searchfn, key);
  //qremove handles making this NULL if not found
  return(match);
}

