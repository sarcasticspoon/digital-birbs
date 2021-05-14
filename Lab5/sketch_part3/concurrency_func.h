/*************************************************************************
 *
 *  Copyright (c) 2013--2018 Bradley and Ivy
 *  All Rights Reserved
 *
 **************************************************************************
 */
#ifndef __CONCURRENCY_FUNC_H__
#define __CONCURRENCY_FUNC_H__

#include "concurrency.h"
#include <time.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------

   THE FOLLOWING FUNCTIONS MUST BE PROVIDED.

------------------------------------------------------------------------*/

/* ====== Part 1 ====== */

__attribute__((used)) unsigned int process_select (unsigned int cursp);
/* Called by the runtime system to select another process.
   "cursp" = the stack pointer for the currently running process
*/

void process_start (void);
/* Starts up the concurrent execution */

int process_create (void (*f)(void), int n);
/* Create a new process with default priority */

int process_create_prio (void (*f)(void), int n, unsigned char prio);
/* Create a new process with user selected priority*/

int process_create_rtjob (void (*f)(void), int n, unsigned int wcet, unsigned int deadline);
/* Create a new process with realtime constraints */

/* ===== Part 2 ====== */

typedef struct lock_state lock_t;
  /* you have to define the lock_state structure */

void lock_init (lock_t *l);
void lock_acquire (lock_t *l);
void lock_release (lock_t *l);

/*-- functions provided in the .c file --*/
//unsigned int process_init (void (*f) (void), int n);
//void process_begin ();
//void yield ();

#ifdef __cplusplus
}
#endif

#endif /* __CONCURRENCY_FUNC_H__ */

