#define LED1 10
#define LED2 11

#include "concurrency_func.h"

extern "C" { extern lock_t* serial_lock; }

/*
 * P1 - P4 each prints the process name and the loop iteration 
 * after a different delay for a different number of iterations
 */
void p1(){
  asm volatile ("cli\n\t");
  lock_acquire(serial_lock);
  delay(9);
  Serial.println("P1 done");
  lock_release(serial_lock);
  asm volatile ("sei\n\t");
  return;
}

void p2(){
  asm volatile ("cli\n\t");
  lock_acquire(serial_lock);
  delay(9);
  Serial.println("P2 done");
  lock_release(serial_lock);
  asm volatile ("sei\n\t");
  return;
}

// test a process with much longer wcet and that is feasible
int test1_setup(){
  lock_acquire(serial_lock);
  Serial.println("1 rt job that is feasible");
  if (serialEventRun) serialEventRun();
  lock_release(serial_lock);
  // stack space 128, wcet 50, deadline is 100 millis after posted
  if(process_create_rtjob(p1, 128, 50, 100) < 0) {
    return -1;
  }
  return 0;
}

// test a process with a shorter deadline than wcet. should not be feasible
int test2_setup(){
  lock_acquire(serial_lock);
  Serial.println("1 rt job that is not feasible");
  if (serialEventRun) serialEventRun();
  lock_release(serial_lock);
  // stack space 128, wcet 10, deadline is 9 millis after posted
  if(process_create_rtjob(p1, 128, 10, 9) < 0) {
    return -1;
  }
  return 0;
}

// four processes that are feasible
int test3_setup(){
  lock_acquire(serial_lock);
  Serial.println("4 rt jobs that are feasible");
  if (serialEventRun) serialEventRun();
  lock_release(serial_lock);
  // stack space 128, wcet 20, deadline is 60 millis after posted
//  if(process_create_rtjob(p2, 128, 20, 60) < 0) {
//    return -1;
//  }
//  if(process_create_rtjob(p2, 128, 20, 40) < 0) {
//    return -1;
//  }
  process_create_rtjob(p1, 128, 10, 50);
  process_create_rtjob(p2, 128, 10, 60);
//  if( < 0) {
//    return -1;
//  }
//  if(process_create_rtjob(p1, 128, 10, 20) < 0) {
//    return -1;
//  }
  return 0;
}

/*
 * call the appropriate test setup function inside the setup function
 * to see different test cases
 */
void setup() {
  asm volatile ("cli\n\t");
  Serial.begin(9600);
  serial_lock = (lock_t*) malloc(sizeof(lock_t));
  lock_init(serial_lock);
  if(test3_setup() < 0) {
    asm volatile ("sei\n\t");
    return;
  }
  asm volatile ("sei\n\t");
}

void loop() {
  process_start();
  while(1) {
    Serial.println("spinning");
    delay(1000);
  }
}
