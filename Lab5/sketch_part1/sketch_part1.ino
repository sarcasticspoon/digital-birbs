#define LED1 10
#define LED2 11

#include "concurrency_func.h"
/*
 * P1 - P4 each prints the process name and the loop iteration 
 * after a different delay for a different number of iterations
 */
void p1(){
  for(int i = 0; i < 100; i++){
    delay(1);
    Serial.print("P1:");
    Serial.println(i);
  }
  return;
}

void p2(){
  for(int i = 0; i < 100; i++){
    delay(2);
    Serial.print("P2:");
    Serial.println(i);
  }
  if(process_create(p2, 64) < 0) {
    return;
  }
  return;
}

void p3(){
  for(int i = 0; i < 50; i++){
    delay(4);
    Serial.print("P3:");
    Serial.println(i);
  }
  if(process_create(p1, 64) < 0) {
    return;
  }
  return;
}

void p4(){
   for(int i = 0; i < 50; i++){
    delay(1);
    Serial.print("P3:");
    Serial.println(i);
  }
  return;
}

/* 
 *  we expect to see P3 print 1 - 50, then terminate
 *  then P3 is responsible for creating P1, which will then print 1 - 100 
 *  until it terminates and the main process will then print 'spinning' continuously
 *  This demonstrates that process_create can be called anytime
*/
int test1_setup() {
  Serial.println("Testing P3 creating process P1");
  if(process_create(p3, 64) < 0) {
    return -1;
  }
  return 0;
}


/* 
 *  we expect to see P2 print 1 - 100, then continuously loop
 *  since P2 creates an identical process just before it terminates.
 *  This demonstrates that "free" is freeing the memory, allowing
 *  P2 to "recycle" the memory that has been allocated to it, meaning
 *  it will never stop looping.
*/
int test2_setup() {
  Serial.println("Testing process P2 recursing");
  if(process_create(p2, 64) < 0) {
    return -1;
  }
  return 0;
}


/*
 * call the appropriate test setup function inside the setup function
 * to see different test cases
 */
void setup() {
  Serial.begin(9600);
  if(test2_setup() < 0) {
    return;
  }
}

void loop() {
  process_start();
  while(1) {
    Serial.println("spinning");
    delay(1000);
  }
}
