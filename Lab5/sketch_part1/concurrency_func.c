include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include "concurrency_func.h"

/* Called by the runtime system to select another process.
   "cursp" = the stack pointer for the currently running process
*/

/*
 * process_select takes the current stack pointer as an argument
 * it returns the stack pointer for the next process to run 
 * 
 * the process first checks to see if there are any processes on the ready queue
 * if not, it returns the current stack pointer without changing the current_process
 * 
 * if the current stack pointer is 0, meaning there is no current running process
 * the function pops the first process of the head of the stack off and saves it as the current_process
 * it increments the ready queue, and returns the previous head's stack pointer
 */
__attribute__((used)) unsigned int process_select (unsigned int cursp)
{
    // if no ready processes, continue with current process, return current sp
    if (!head) {
      return cursp;         
    }

    //if no current process, don't add anything to queue, just pop off head for current process
    if (cursp == 0) {
      // set current_process to head of queue
        current_process = head;
      // advance the queue
        head = head->next;
      // NULL out current process' next variable 
        current_process->next = NULL;
      // return current process' sp
        return current_process->sp;
      //dont add current process to the back of the queue.
    }

    // else if queue is not empty and there is a current process
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

    // return current_process' sp
    return current_process->sp;
}

/* process_start starts up the concurrent execution */
void process_start (void) {
  // intialize the current_process variable
    current_process = NULL;
    process_begin();
};


/* 
 *  process_create creates a new process_t, 
 *  reserves space for the process' stack, 
 *  and adds the process_t to the ready queue
*/
int process_create (void (*f)(void), int n) {
    // disable interrupts
    asm volatile("cli\n\t");
    unsigned int sp;
    // malloc space for the process to go into the ready queue
    process_t * proc = process_malloc(sizeof(process_t));
    if(!proc) {
        return -1;
    }
    // call process_init to set up the stack for this proccess
    if((sp = process_init (f, n)) == 0) {
        return -1;
    };
    // initialize values for process, add to tail of linked list
    proc->sp = sp;
    proc->next = NULL;
    if(!head) {
      head = proc; 
    } else {
      process_t *tail = head;
      while (tail->next) {
          tail = tail->next;
      }
      tail->next = proc;
    }
    // enable interrupts again
    asm volatile("sei\n\t");
    return 0;
}
