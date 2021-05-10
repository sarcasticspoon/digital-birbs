#ifndef __CONDITION_H__
#define __CONDITION_H__

#include "concurrency.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct cond_state {
  process_t* head;
  lock_t* lock;
} cond_t;

void cond_init (lock_t *m, cond_t *c);
void cond_wait (lock_t *m, cond_t *c);
void cond_signal (lock_t *m, cond_t *c);
int cond_waiting (lock_t *m, cond_t *c);

#ifdef __cplusplus
}
#endif

#endif /* __CONDITION_H__ */
