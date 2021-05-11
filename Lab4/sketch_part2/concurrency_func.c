#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include "concurrency_func.h"

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
    if (cursp == 0) {
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
    } // OR just use tail

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
    proc->s e("sei\n\t");
    return 0;
}

void lock_init (lock_t *l) {
  asm volatile("cli\n\t");
  // l = (lock_t*) malloc(sizeof(lock_t));
  l->lock = false; 
  asm volatile("sei\n\t");
  return;
}

void lock_acquire (lock_t *l){
  asm volatile("cli\n\t");
  while (l->lock) {
    yield();
  }

  l->lock = true;
  asm volatile("sei\n\t");
}


void lock_release (lock_t *l){
  asm volatile("cli\n\t");
  l->lock = false;
  asm volatile("sei\n\t");
}


/*
__attribute__((used)) unsigned int process_select (unsigned int cursp)
{

    // if no ready processes, continue with current process
    if (!head) {
      return cursp;         
    }

    process_t* next = head;
    // if no current process, don't add anything to queue
    if (cursp == 0) {
      current_process->sp = head->sp;
      head = head->next;
      free(next);
      //dont add current process to the back of the queue.
    } else {
        //swap sp
        current_process->sp = head->sp;
        head->sp = cursp;

        //push original head back to the end of the queue
    
        //advance the head pointer down the queue
        head = head->next;

        // find the end of the process queue
        process_t *end = head;
        while (end->next) {
            end = end->next;
        }

        //assign head to the back of the list, and kill its original next pointer.
        end->next = next;
        next->next = NULL;
    }

    return current_process->sp;
}
*/
