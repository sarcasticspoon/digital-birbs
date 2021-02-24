#include "testasm.h"

/* the attribute "used" tells the compiler to not inline this function */
__attribute__((used))
unsigned int fib (unsigned char a) {
  unsigned int out;

  asm volatile (R"(

   rjmp start_of_assembly

   .data
   ; any data values/static storage can go here

   .text

fib_recurse:
   ;first argument (n) is in r24
   ldi r25,2   ;value to compare n to
   cp r24,r25  ;check if n is less than 2
   brlo less_than_two ;branch if r24 < 2
more_than_two:
   push r24          ;save n to the stack
   subi r24,1        ;get n - 1
   call fib_recurse  ;call fib(n-1)
   mov r22,r24       ;put fib(n-1) in r22
   pop r24           ;put r24 = n
   subi r24,2        ;get n-2
   push r22          ;save fib(n-1) on the stack
   call fib_recurse  ;call fib(n-2)
   pop r22           ;get back fib(n-2)
   add r24,r22       ;add fib(n-1) + fib(n-2)
   ret               
less_than_two:
   clr r25
   ret

start_of_assembly:
   mov r24,%1
   call fib_recurse

end_of_assembly: 
   ; -- move r25:r24 to the 16-bit word in variable out
   clr r25
   movw %0,r24

)" : "=w" (out)  /* out is %0, and a 16-bit variable that is modified
		    by the assembly language */
   : "r" (a)  /* a is %1, b is %2, both are register-sized
			  variables */
   : "r25", "r24");   /* r24, r25 are modified by this assembly
			 language fragment */

  return out;
}
