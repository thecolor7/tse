/* 
 * lhash.h --- 
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>

// structure definition
typedef struct lhash lhash_t;

// Function prototypes
lhash_t* lhopen(uint32_t hsize);
int32_t lhput(lhash_t *lhp, void *elementp, const char *key, int keylen);
void lhapply(lhash_t *lhp, void (*fn)(void* elementp));
void* lhsearch(lhash_t *lhp, bool (*searchfn)(void* elementp, const void* keyp), const void* key, int32_t keylen);
void lhclose(lhash_t *lhp);
void* lhremove(lhash_t *lhp, bool (*searchfn)(void* elementp, const void* searchkeyp), const char *key, int32_t keylen);
