#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include "concurrency.h"

__attribute__((used)) unsigned char _orig_sp_hi, _orig_sp_lo;

__attribute__((used)) unsigned char _save_sp_hi, _save_sp_lo;

__attribute__((used)) unsigned char _free_sp_hi, _free_sp_lo;

__attribute__((used)) unsigned char _n_hi, _n_lo;

__attribute__((used)) void *process_malloc(int sz)
{
  if (!current_process) return (void *)malloc (sz);
 asm volatile ( "in r23,__SP_L__ \n\t"
                "sts _save_sp_lo, r23\n\t"
                "in r23,__SP_H__ \n\t"
                "sts _save_sp_hi, r23\n\t"
                "lds r23, _orig_sp_hi\n\t"
                "out __SP_H__, r23\n\t"
                "lds r23, _orig_sp_lo\n\t"
                "out __SP_L__, r23\n\t"
                "call malloc\n\t"
                "lds r23, _save_sp_hi\n\t"
                "out __SP_H__, r23\n\t"
                "lds r23, _save_sp_lo\n\t"
                "out __SP_L__, r23\n\t"
                "ret\n\t");
}

__attribute__((used)) void process_begin ()
{
  asm volatile (
		"cli \n\t"
		"in r24,__SP_L__ \n\t"
		"sts _orig_sp_lo, r24\n\t"
		"in r25,__SP_H__ \n\t"
		"sts _orig_sp_hi, r25\n\t"
		"ldi r24, 0\n\t"
		"ldi r25, 0\n\t"
		"rjmp .dead_proc_entry\n\t"
		);
}

__attribute__((used)) void process_terminated ()
{
  asm volatile (
		"cli\n\t"
		"pop r0\n\t"
		"sts _n_hi, r0\n\t"
		"pop r0\n\t"
		"sts _n_lo, r0\n\t"
		"in r25, __SP_H__\n\t"
		"sts _free_sp_hi, r25\n\t"
		"in r24, __SP_L__\n\t"
		"sts _free_sp_lo, r24\n\t"
  );

  double diff = 0;
  // check to see if wcet is accurate
  double time_elapsed = (double) clock();
  time_elapsed = time_elapsed / CLOCKS_PER_SEC * 1000; // convert to milliseconds
  time_elapsed = time_elapsed - current_process->start;
  // check to see if wcet is accurate
  diff = current_process->wcet - time_elapsed;
  // TODO we need to report the difference, negative means took longer than expected
  // TODO check to see if we missed the deadline and if it's because of this difference
  double deadline_diff = current_process->deadline - ((double) clock() / CLOCKS_PER_SEC * 1000);
  if(deadline_diff < 0) {
    // means that we missed the deadline
    // check to see if the difference can be accounted for by the difference between wcet and actual execution time
    if(diff < 0) {
      if(deadline_diff - diff >= 0) {
        // means we missed deadline because of wcet difference
        // ie wcet difference is greater or equal to deadline miss
        // TODO report this somehow
      }
    }
  }
  
  uintptr_t stack_addr = _free_sp_hi << 8 + _free_sp_lo;
  uintptr_t n = _n_hi << 8 + _n_lo;
  uintptr_t base_addr = stack_addr - n;
  free((unsigned char*)base_addr);
  free(current_process);
  
  asm volatile (
		"lds r25, _orig_sp_hi\n\t"
		"out __SP_H__, r25\n\t"
		"lds r24, _orig_sp_lo\n\t"
		"out __SP_L__, r24\n\t"
		"ldi r24, lo8(0)\n\t"
		"ldi r25, hi8(0)\n\t"
		"rjmp .dead_proc_entry"
		);
}

void process_timer_interrupt ();

__attribute__((used)) void yield ()
{
  if (!current_process) return;
  asm volatile ("cli\n\t");
  asm volatile ("rjmp process_timer_interrupt\n\t");
}

__attribute__((used)) void process_timer_interrupt()
{
  asm volatile (
		"push r31\n\t"
		"push r30\n\t"
		"push r29\n\t"
		"push r28\n\t"
		"push r27\n\t"
		"push r26\n\t"
		"push r25\n\t"
		"push r24\n\t"
		"push r23\n\t"
		"push r22\n\t"
		"push r21\n\t"
		"push r20\n\t"
		"push r19\n\t"
		"push r18\n\t"
		"push r17\n\t"
		"push r16\n\t"
		"push r15\n\t"
		"push r14\n\t"
		"push r13\n\t"
		"push r12\n\t"
		"push r11\n\t"
		"push r10\n\t"
		"push r9\n\t"
		"push r8\n\t"
		"push r7\n\t"
		"push r6\n\t"
		"push r5\n\t"
		"push r4\n\t"
		"push r3\n\t"
		"push r2\n\t"
		"push r1\n\t"
		"push r0\n\t"
		"in r24, __SREG__\n\t"
		"push r24\n\t"
		"in r24, __SP_L__\n\t"
		"in r25, __SP_H__\n\t"
		".dead_proc_entry:\n\t"
		"rcall process_select\n\t"
		"eor r18,r18\n\t"
		"or r18,r24\n\t"
		"or r18,r25\n\t"
		"brne .label_resume\n\t"
		"lds r25, _orig_sp_hi\n\t"
		"out __SP_H__, r25\n\t"
		"lds r24, _orig_sp_lo\n\t"
		"out __SP_L__, r24\n\t"
		"ret\n\t"
		".label_resume:\n\t"
		"out __SP_L__, r24\n\t"
		"out __SP_H__, r25\n\t"
		"pop r0\n\t"
		"out  __SREG__, r0\n\t"
		"pop r0\n\t"
		"pop r1\n\t"
		"pop r2\n\t"
		"pop r3\n\t"
		"pop r4\n\t"
		"pop r5\n\t"
		"pop r6\n\t"
		"pop r7\n\t"
		"pop r8\n\t"
		"pop r9\n\t"
		"pop r10\n\t"
		"pop r11\n\t"
		"pop r12\n\t"
		"pop r13\n\t"
		"pop r14\n\t"
		"pop r15\n\t"
		"pop r16\n\t"
		"pop r17\n\t"
		"pop r18\n\t"
		"pop r19\n\t"
		"pop r20\n\t"
		"pop r21\n\t"
		"pop r22\n\t"
		"pop r23\n\t"
		"pop r24\n\t"
		"pop r25\n\t"
		"pop r26\n\t"
		"pop r27\n\t"
		"pop r28\n\t"
		"pop r29\n\t"
		"pop r30\n\t"
		"pop r31\n\t"
		"reti\n\t");
}


/*
 * Stack: save 32 regs, +2 for entry point +2 for ret address, +2 for stack size
 */
#define EXTRA_SPACE 39
#define EXTRA_PAD 4

unsigned int process_init (void (*f) (void), int n)
{
  unsigned long stk;
  int i;
  unsigned char *stkspace;
  /* TODO disable and enable interrupts around malloc */
  /* Create a new process */
  n += EXTRA_SPACE + EXTRA_PAD;
  stkspace = (unsigned char *) process_malloc (n);

  if (stkspace == NULL) {
    /* failed! */
    return 0;
  }

  /* Create the "standard" stack, including entry point */
  for (i=0; i < n; i++) {
      stkspace[i] = 0;
  }

  n -= EXTRA_PAD;

  stkspace[n-1] = n & 0xff;
  stkspace[n-2] = n >> 8;
  stkspace[n-3] = ( (unsigned int) process_terminated ) & 0xff;
  stkspace[n-4] = ( (unsigned int) process_terminated ) >> 8;
  stkspace[n-5] = ( (unsigned int) f ) & 0xff;
  stkspace[n-6] = ( (unsigned int) f ) >> 8;

  /* SREG */
  stkspace[n-EXTRA_SPACE] = SREG;

  stk = (unsigned int)stkspace + n - EXTRA_SPACE - 1;

  return stk;
}