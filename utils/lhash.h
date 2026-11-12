/* 
 * hash.h -- A generic hash table implementation, allowing arbitrary
 * 
 * Author: Eve H. Belding
 * Created: 11-11-2026
 * Version: 1.0
 * 
 */

#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct lhash lhash_t; /* representation of a hashtable hidden */

/* hopen -- opens a hash table with initial size hsize */
lhash_t* lhopen(uint32_t hsize);

/* hclose -- closes a hash table */
void lhclose(lhash_t *lhp);

/* hput -- puts an entry into a hash table under designated key
 * returns 0 for success; non-zero otherwise
 */
int32_t lhput(lhash_t *lhp, void *elementp, const char *key, int keylen);

/* happly -- applies a function to every entry in hash table */
void lhapply(lhash_t *lhp, void (*fn)(void* elementp));

/* hsearch -- searchs for an entry under a designated key using a
 * designated search fn -- returns a pointer to the entry or NULL if
 * not found
 */
void* lhsearch(lhash_t *lhp, bool (*searchfn)(void* elementp, const void* keyp), const void* key, int32_t keylen) ;

void *lhremove(lhash_t *lhp, bool (*searchfn)(void* elementp, const void* searchkeyp), const char *key, int32_t keylen);

