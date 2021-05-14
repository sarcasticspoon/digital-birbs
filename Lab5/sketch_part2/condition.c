#include "condition.h"
#include <stddef.h>

/* initialize condition variable queue and associate it with lock */
void cond_init (lock_t *m, cond_t *c) {
  c->lock = m;
  c->head = NULL;
}

/* put a process on the waiting queue for the condition variable */
void cond_wait (lock_t *m, cond_t *c) {
  // check to see if the lock is right
  if(m != c->lock) {
    // error, return 
    return;
  }
  asm volatile("cli\n\t");
  // put yourself on the queue and yield
  current_process->is_waiting = 1;
  process_t* tail = c->head;
  if(!tail) {
    c->head = current_process;
  } else {
    while(tail->next) {
      tail = tail->next;
    }
    tail->next = current_process;
  }
  // release the lock
  lock_release(m);
  asm volatile("sei\n\t");
  yield();
  // reacquire the lock after coming back
  lock_acquire(m);
}

/* wake up the first process that is waiting on this condition variable */
/* assume that this is called after checking that there is a process waiting */
void cond_signal (lock_t *m, cond_t *c) {
  // check to see if lock is the same
  if(m != c->lock) {
    // error, return 
    return;
  }
  asm volatile("cli\n\t");
  // wake up next process
  process_t* waiting_process = c->head;
  c->head = c->head->next;
  waiting_process->is_waiting = 0;
  process_add(waiting_process);
  // release your lock
  lock_release(m);
  asm volatile("sei\n\t");
  return;
 }

/* returns 0 if no processes waiting; 1 otherwise */
int cond_waiting (lock_t *m, cond_t *c) {
  // check to see if the lock is the same
  if(m != c->lock) {
    return 1;
  }
  if(c->head) {
    return 1;
  }
  return 0;
}
