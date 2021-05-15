#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include "concurrency_func.h"
#include "log.h"

extern lock_t* serial_lock;

/* Called by the runtime system to select another process.
   "cursp" = the stack pointer for the currently running process
*/
__attribute__((used)) unsigned int process_select (unsigned int cursp)
{
    // if no ready processes, continue with current process
    if (!head) {
      return cursp;         
    }
    
    //if no current process, don't add anything to queue 
    if (cursp == 0 || current_process->is_waiting) {
      // save the current stack pointer of the waiting process
      if(cursp != 0) {
        current_process->sp = cursp;
      }
        current_process = head;
      // advance the queue
        head = head->next;
      // NULL out current process' next variable 
        current_process->next = NULL;
        return current_process->sp;
      //dont add current process to the back of the queue.
    }

    // find the end of the process queue
    process_t *end = head;
    while (end->next) {
        end = end->next;
    }

    // save the cursp into the current process
    current_process->sp = cursp;

    // add the current process to the back of the queue
    end->next = current_process;

    // set current_process to next ready process
    current_process = head;

    // advance the queue
    head = head->next;

    // NULL out current process' next variable
    current_process->next = NULL;

    // grab return value if any
    return current_process->sp;
}

// takes a process_t pointer and adds the process to the end of the queue
// called in cond_signal to wake up the next process
void process_add(process_t *p) {
  process_t* tail = head;
  // if no other processes, set this process to the head
  if(!head) {
    head = p;
  } else {
    // set process to end of the queue
    while(tail->next) {
      tail = tail->next;
    }
    tail->next = p;
  }
  // null out this process' next variable
  p->next = NULL;
  return;
}

/* Starts up the concurrent execution */
void process_start (void) {
    current_process = NULL;
    process_begin();
};


/* Create a new process */
int process_create (void (*f)(void), int n) {
    asm volatile("cli\n\t");
    unsigned int sp;
    process_t * proc = malloc(sizeof(process_t));
    if(!proc) {
        return -1;
    }
    if((sp = process_init (f, n)) == 0) {
        return -1;
    };
    proc->sp = sp;
    proc->next = NULL;
    proc->is_waiting = 0;
    if(!head) {
      head = proc; 
    } else {
      process_t *tail = head;
      while (tail->next) {
          tail = tail->next;
      }
      tail->next = proc;
    }
    asm volatile("sei\n\t");
    return 0;
}

// initializes the lock to an unlocked state
void lock_init (lock_t *l) {
  asm volatile("cli\n\t");
  l->lock = false; 
  asm volatile("sei\n\t");
  return;
}

// a process continually yields until it can lock the lock
void lock_acquire (lock_t *l){
  asm volatile("cli\n\t");
  // if locked, yield and then try again
  while (l->lock) {
    yield();
    asm volatile("cli\n\t");
  }
  // set the lock to locked
  l->lock = true;
  asm volatile("sei\n\t");
}

// set the lock to an unlocked state
void lock_release (lock_t *l){
  asm volatile("cli\n\t");
  l->lock = false;
  asm volatile("sei\n\t");
}
