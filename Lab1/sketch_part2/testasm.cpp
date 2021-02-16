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

  ; assume that a & b are passed in r20 r21
  ; return value in r24 r25
start_of_sum:
   mov r24, r20
   mov r25, r21
   ; We add r25 to r24, clearing r25 and branching conditional on the carry bit
   add r24,r25
   clr r25
   brcc end_of_sum
   ldi r25,1 
end_of_sum:
  ret

  ; assume that a & b are passed in r20 r21
  ; return value in r24 r25
start_of_diff:
  mov r24,r20
  mov r25,r21
  ; compare r24 and r25
   cp r24,r25
   brcs b_greater
   sub r24,r25
   jmp end_of_diff
b_greater:
   sub r25,r24
   mov r24,r25  
end_of_diff:
   clr r25
   ret

start_of_main:
   ; We adopt a similar approach in Part 1 of the lab
   ; Move arguments a and b into register r20 and r21
   mov r20, %1
   mov r21, %2
   call start_of_sum
   push r24
   push r25
   call start_of_diff 
   ; now diff return val in r24
   pop r25 ; move higher order bits from a+b into r25
   pop r0 ; move lower order bits from a+b into r0
   add r24,r0
   brcc end_of_main
   add r25,1
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

///* the attribute "used" tells the compiler to not inline this function */
//__attribute__((used))
//unsigned int diffval (unsigned char a, unsigned char b) {
//  unsigned int out;
//
//  asm volatile (R"(
//
//   rjmp start_diffval
//
//   .data
//   ; any data values/static storage can go here
//
//   .text
//
//start_diffval:
//   ; Move arguments a and b into register r24 and r25
//   mov r24,%1
//   mov r25,%2
//   ;
//   ; Comparing which value is greater, subtract the smaller
//   ; value from the greater value.
//   cp r24,r25
//   brcs b_greater
//   sub r24,r25
//   jmp end_diffval
//b_greater:
//   sub r25,r24
//   mov r24,r25
//   ;
//end_diffval: 
//   ; -- move r25:r24 to the 16-bit word in variable out
//   ; Remember that since we are subtracting 8-bit number from another,
//   ; the maximum value is 255, so we always clear r25.
//   clr r25
//   movw %0,r24
//
//)" : "=w" (out)  /* out is %0, and a 16-bit variable that is modified
//        by the assembly language */
//   : "r" (a), "r" (b)  /* a is %1, b is %2, both are register-sized
//        variables */
//   : "r25", "r24");   /* r24, r25 are modified by this assembly
//       language fragment */
//
//  return out;
//}
