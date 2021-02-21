#include "testasm.h"

/* the attribute "used" tells the compiler to not inline this function */
__attribute__((used))
unsigned int sum_and_diff (unsigned char a, unsigned char b) {
  unsigned int out;

  asm volatile (R"(

   rjmp start_of_main

   .data
   ; any data values/static storage can go here

   .text

  ; assume that a & b are passed in r24 r22
  ; return value in r24 r25
start_of_sum:
   ; We add r22 to r24, clearing r25 and using adc to account for the carry
   add r24,r22
   clr r25
   adc r25,r25
   ret

  ; assume that a & b are passed in r24 r22
  ; return value in r24 r25
start_of_diff:
  ; compare r24 and r22
   cp r24,r22
   brcs b_greater
   sub r24,r22
   jmp end_of_diff
b_greater:
   sub r22,r24
   mov r24,r22  
end_of_diff:
   clr r25
   ret

start_of_main:
   ; We adopt a similar approach in Part 1 of the lab
   ; Move arguments a and b into register r20 and r21
   mov r24, %1
   mov r22, %2
   call start_of_sum
   push r24
   push r25
   mov r24, %1
   mov r22, %2
   call start_of_diff 
   ; now diff return val in r24
   pop r25 ; move higher order bits from a+b into r25
   pop r0 ; move lower order bits from a+b into r0
   add r24,r0
   adc r25,r1
   ;
end_of_main: 
   ; -- move r25:r24 to the 16-bit word in variable out
   movw %0,r24

)" : "=w" (out)  /* out is %0, and a 16-bit variable that is modified
		    by the assembly language */
   : "r" (a), "r" (b)  /* a is %1, b is %2, both are register-sized
			  variables */
   : "r25", "r24");   /* r24, r25 are modified by this assembly
			 language fragment */

  return out;
}
