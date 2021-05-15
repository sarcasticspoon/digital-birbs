#ifndef __CONDITION_H__
#define __CONDITION_H__

#include "concurrency_func.h"

#ifdef __cplusplus
extern "C" {
#endif

// condition variable type, 
// holds pointer to a queue of waiting processes and an associated lock
typedef struct cond_state {
  volatile process_t* head;
  lock_t* lock;
} cond_t;

void cond_init (lock_t *m, cond_t *c); /* intializes the cv */
void cond_wait (lock_t *m, cond_t *c); /* waits on cv */
void cond_signal (lock_t *m, cond_t *c); /* signals next process waiting on cv */
int cond_waiting (lock_t *m, cond_t *c); /* checks if there is a waiting process on the cv */

#ifdef __cplusplus
}
#endif

#endif /* __CONDITION_H__ */
