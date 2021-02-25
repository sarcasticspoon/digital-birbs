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

; first argument n is given in r24
; fib_recurse returns value in r24
fib_recurse:
   ldi r25,2          ;load r25 with constant 2 
   cp r24,r25         ;check if n is less than r25 (value 2)
   brlo less_than_two ;branch if r24 < 2
more_than_two:
   push r24          ;save n to the stack
   subi r24,1        ;get n - 1
   call fib_recurse  ;call fib(n-1)
   mov r22,r24       ;move result from fib(n-1) in r24 to r22
   pop r24           ;load r24 with stored n from the stack
   subi r24,2        ;get n-2
   push r22          ;save fib(n-1) on the stack
   call fib_recurse  ;call fib(n-2)
   pop r22           ;restore fib(n-2) from the stack
   add r24,r22       ;add fib(n-1) + fib(n-2) and store in r24
   ret               
less_than_two:
   clr r25 ; clear r25, previously loaded with 2
   ret

start_of_assembly:
   mov r24,%1       ;move first argument into r24
   call fib_recurse 

end_of_assembly: 
   ; -- move r25:r24 to the 16-bit word in variable out
   clr r25
   movw %0,r24

)" : "=w" (out)  /* out is %0, and a 16-bit variable that is modified
		    by the assembly language */
   : "r" (a)  /* a is %1,*/
   : "r25", "r24");   /* r24, r25 are modified by this assembly
			 language fragment */

  return out;
}
