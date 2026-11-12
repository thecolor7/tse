/* 
 * lqueue.h --- 
 */

#pragma once
#include <stdbool.h>
#include <stdint.h>

// structure definition
typedef struct lqueue lqueue_t;

// Function prototypes
lqueue_t* lqopen(void);
int32_t lqput(lqueue_t *lqp, void *elementp);
void* lqget(lqueue_t *lqp);
void lqapply(lqueue_t *lqp, void (*fn)(void* elementp));
void* lqsearch(lqueue_t *lqp, bool (*searchfn)(void* elementp, const void* keyp), const void* skeyp);
void lqclose(lqueue_t *lqp);
