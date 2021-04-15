#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include "concurrency_func.h"

/* Called by the runtime system to select another process.
   "cursp" = the stack pointer for the currently running process
*/
__attribute__((used)) unsigned int process_select (unsigned int cursp)
{
    if (cursp == 0) {
            current_process = NULL;
        }

    if (!head) {
        return cursp;         
    }

    /* find the end of the process queue*/ 
    process_t *end = head;
    while (end->next) {
        end = end->next;
    } // OR just use tail

    /* add the current process to the back of the queue */
    // assume here that current_process->sp == cursp. 
    end->next = current_process;

    /* set current_process to next ready process */
    current_process = head;

    /* NULL out current process' next variable */
    current_process->next = NULL;

    /* advance the queue */
    head = head->next;

    /* grab return value if any */ 
    return current_process->sp;
}

/* Starts up the concurrent execution */
void process_start (void) {
    current_process = NULL;
    process_begin();
};


/* Create a new process */
int process_create (void (*f)(void), int n) {
    unsigned int sp;
    process_t * proc = malloc(sizeof(process_t));
    if(!proc) {
        return -1;
    }
    if((sp = process_init (f, n)) == 0) {
        return -1;
    };
    proc->sp = sp;
    proc->next = head;
    head = proc;
    return 0;
}
