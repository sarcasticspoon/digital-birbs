#include "condition.h"

void cond_init (lock_t *m, cond_t *c) {
  // not really sure what they intend to do here. maybe initialize things?
  // how to associate lock with c? 
}

/* put a process on the waiting queue for the condition variable */
void cond_wait (lock_t *m, cond_t *c) {
  // unlock the lock
  // put yourself on the queue and yield
  // lock
}

/* wake up the first process that is waiting on this condition variable */
/* assume that this is called after checking that there is a process waiting */
void cond_signal (lock_t *m, cond_t *c) {
  lock_release(m);
  process_t* waiting_process = c->head;
  c->head = c->head->next;
  // unlock
  // put another process on the ready queue from the waiting queue
}

/* returns 0 if no processes waiting; 1 otherwise */
int cond_waiting (lock_t *m, cond_t *c) {
  if(c->head) {
    return 1;
  }
  return 0;
}
