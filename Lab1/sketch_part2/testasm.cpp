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

  ; a & b are passed in r24 r22
  ; return value in r25:r24
  ; this is a leaf function, so we only use caller-saved registers
start_of_sum:
   add r24,r22 ; add r22 to r24, storing the value in r24
   clr r25     ; clear r25, the higher order bits of r25:r24 pair used to return value
   adc r25,r25 ; add r25 to itself (0+0) with the carry bit from the previous add operation
   ret         ; return to caller

  ; assume that a & b are passed in r24 r22
  ; return value in r25:r24
  ; this is also a leaf function, so we only use caller-saved registers
start_of_diff:
   cp r24,r22       ; compare r24 and r22, setting carry flag
   brcs b_greater   ; if carry flag is set, r22's value was greater so jump to b_greater
   sub r24,r22      ; else, subtract r22 from r24 and store value in r24
   jmp end_of_diff  ; jump to end_of_diff to return value
b_greater:
   sub r22,r24      ; subtract r24 from r22 and store value in r22
   mov r24,r22      ; move the difference from r22 to r24
end_of_diff:
   clr r25          ; clear r25 since the sum will never exceed 8 bits
   ret              ; return to caller 

start_of_main:
   ; We adopt a similar approach in Part 1 of the lab
   ; Move arguments a and b into register r24 and r22
   mov r24, %1
   mov r22, %2
   ; call sum function with parameters a & b in r24 and r22
   call start_of_sum
   ; push the return value of sum function onto the stack
   ; first push lower order bits, then higher order bits of a+b
   push r24
   push r25
   ; Move arguments a and b into register r24 and r22 again
   mov r24, %1
   mov r22, %2
   ; call diff function with parameters a & b in r24 and r22
   call start_of_diff 
   ; the return value of diff function is now in r24
   pop r25    ; move higher order bits from a+b into r25
   pop r0     ; move lower order bits from a+b into r0
   add r24,r0 ; add r0 to r24; ie add the lower order bits from a+b to a-b (assume a > b)
   adc r25,r1 ; add r1 (value of 0) to r25 with the value of the carry bit from the last sum operation
   ; if a+b + a-b has no carry, r25 will simply retain the its value
   ; if it does have a carry bit, that will be added to r25
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
