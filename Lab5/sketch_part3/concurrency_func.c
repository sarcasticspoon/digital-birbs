#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include "concurrency_func.h"
#include "log.h"

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
  //test! 
  mlog("calling_process_select");
  mlog((char*)cursp);
  // disable interrupts outside
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
    
    //if next processes priority is lower than current process, do nothing
    if(head->prio > current_process->prio) {
        current_process->sp = cursp;
        return current_process->sp;
    }

    // else if queue is not empty and there is a current process
    // find the last element of the matching priority
    unsigned char curr_prio = current_process->prio;
    process_t *last_of_prio = head;
    while (last_of_prio->next) {
        if (last_of_prio->next->prio > curr_prio) break;
        last_of_prio = last_of_prio->next;
    }

    // save the cursp into the current process
    current_process->sp = cursp;

    // add the current process to the middle of the queue after the last process of matching priority
    process_t *tmp = last_of_prio->next;
    last_of_prio->next = current_process;
    last_of_prio->next->next = tmp;

    // set current_process to next ready process
    current_process = head;

    // advance the queue
    head = head->next;

    // NULL out current process' next variable
    current_process->next = NULL;

    //if RT job -> start the timer
    if( current_process->start == 0) {
      current_process->start = (double) clock() / CLOCKS_PER_SEC * 1000;
    }

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
    //set default priority
    proc->prio = 128;
    // default values for real time
    proc->start = -1;
    proc->deadline = -1;
    proc->wcet = 0;
    if(!head) {
      head = proc; 
    } else {
      process_t *last_of_prio = head;
      while (last_of_prio->next) {
        if (last_of_prio->next->prio > proc->prio) break;
        last_of_prio = last_of_prio->next;
      }

      // add the current process to the middle of the queue after the last process of matching priority
      process_t *tmp = last_of_prio->next;
      last_of_prio->next = proc;
      last_of_prio->next->next = tmp;
    }
    // enable interrupts again
    asm volatile("sei\n\t");
    return 0;
}

/* 
 *  process_create creates a new process_t, 
 *  reserves space for the process' stack, 
 *  and adds the process_t to the ready queue
 *  priority is also passed into the function
*/
int process_create_prio (void (*f)(void), int n, unsigned char prio) {
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
    //set user selected priority
    proc->prio = prio;
    // initialize values for process, add to tail of linked list
    proc->sp = sp;
    // why do we need to of these? TODO
    proc->sp = sp;
    proc->next = NULL;
    // default for real time
    proc->start = -1;
    proc->deadline = -1;
    proc->wcet = 0;
    if(!head) {
      head = proc; 
    } else {
      process_t *last_of_prio = head;
      while (last_of_prio->next) {
        if (last_of_prio->next->prio > proc->prio) break;
        last_of_prio = last_of_prio->next;
      }

      // add the current process to the middle of the queue after the last process of matching priority
      process_t *tmp = last_of_prio->next;
      last_of_prio->next = proc;
      last_of_prio->next->next = tmp;
    }
    // enable interrupts again
    asm volatile("sei\n\t");
    return 0;
}

int process_create_rtjob (void (*f)(void), int n, unsigned int wcet, unsigned int deadline) {
  // disable interrupts
  asm volatile("cli\n\t");
  // allocate space for process_t
  unsigned int sp;
  process_t * proc = process_malloc(sizeof(process_t));
  if(!proc) {
      return -1;
  }
  // call process_init to set up the stack for this proccess
  if((sp = process_init(f, n)) == 0) {
      return -1;
  };
  proc->sp = sp;
  proc->prio = 0;
  proc->next = NULL;
  proc->start = 0;
  proc->deadline = ((double) clock()) / CLOCKS_PER_SEC * 1000 + deadline; // convert to milliseconds
  proc->wcet = wcet;
  
  // insert job into appropriate place in queue
  process_t *last_of_rt;
  if(!head) {
      head = proc; 
    } else {
      last_of_rt = head;
      while (last_of_rt->next) {
        if (last_of_rt->next->prio > proc->prio) break;
        if (last_of_rt->deadline < proc->deadline && last_of_rt->next->deadline > proc->deadline) break;
        last_of_rt = last_of_rt->next;
    }

      // add the current process to the middle of the queue after the last process of matching priority
      process_t *tmp = last_of_rt->next;
      last_of_rt->next = proc;
      last_of_rt->next->next = tmp;
    }

  // check to see if schedule is feasible
  process_t* tmp = head;
  // start time is current processes start time + current_proc's wcet
  double start = current_process->wcet + current_process->start; //-(clock()-current_process->start);
  while(tmp->prio == 0) {
    if (tmp->deadline < start + tmp->wcet) {
      //TODO pull out the process from the queue 
      if (last_of_rt) {
        last_of_rt->next = last_of_rt->next->next;
        free(proc);
      } else {
        head = NULL;
      }
      return -1; //schedule is not feasible
    }

    start = start + tmp->wcet;
    tmp = tmp->next;
  }
  
  // if it is, we need some way to set the start of the process
  //we do this in process select

  // enable interrupts 
  asm volatile("sei\n\t");
  return 0;
}
