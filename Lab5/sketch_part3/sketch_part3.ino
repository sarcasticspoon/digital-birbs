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
  if (serialEventRun) serialEventRun();
  Serial.println("P1 start");
  delay(9);
  Serial.println("P1 done");
  lock_release(serial_lock);
  asm volatile ("sei\n\t");
  return;
}

void p2(){
  asm volatile ("cli\n\t");
  lock_acquire(serial_lock);
  if (serialEventRun) serialEventRun();
  Serial.println("P2 start");
  delay(9);
  Serial.println("P2 done");
  lock_release(serial_lock);
  asm volatile ("sei\n\t");
  return;
}

void p3(){
  asm volatile ("cli\n\t");
  lock_acquire(serial_lock);
  if (serialEventRun) serialEventRun();
  Serial.println("P3 start");
  delay(9);
  Serial.println("P3 done");
  lock_release(serial_lock);
  asm volatile ("sei\n\t");
  return;
}

void p4(){
  asm volatile ("cli\n\t");
  lock_acquire(serial_lock);
  if (serialEventRun) serialEventRun();
  Serial.println("P4 start");
  delay(9);
  Serial.println("P4 done");
  lock_release(serial_lock);
  asm volatile ("sei\n\t");
  return;
}

//P5 and P6 just prints out numbers in a loop. This is to demonstrate 
//that the priority queue works. 
void p5(){
  for(int i = 0; i < 100; i++){
    delay(1);
    Serial.print("P5:");
    Serial.println(i);
  }
  return;
}

void p6(){
  for(int i = 0; i < 100; i++){
    delay(1);
    Serial.print("P6:");
    Serial.println(i);
  }
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
  // stack space 128, wcet 60, deadline is 120 millis after posted
  // need to account for time overhead in creating processes
  process_create_rtjob(p1, 128, 60, 120);
  // stack space 128, wcet 60, deadline is 200 millis after posted
  process_create_rtjob(p2, 128, 60, 200);
  // stack space 128, wcet 60, deadline is 300 millis after posted
  process_create_rtjob(p3, 128, 60, 300);
  // stack space 128, wcet 60, deadline is 100 millis after posted
  process_create_rtjob(p4, 128, 60, 400);
  return 0;
}

// three processes that are and one process that causes another process to be infeasible
// that process is rejected and not added to the list
// also note that processes get added in the right order
int test4_setup(){
  lock_acquire(serial_lock);
  Serial.println("4 rt jobs that are feasible");
  if (serialEventRun) serialEventRun();
  lock_release(serial_lock);
  // stack space 128, wcet 60, deadline is 160 millis after posted
  // need to account for time overhead in creating processes
  process_create_rtjob(p1, 128, 60, 185);
  // stack space 128, wcet 60, deadline is 300 millis after posted
  process_create_rtjob(p3, 128, 60, 300);
  // stack space 128, wcet 60, deadline is 200 millis after posted
  process_create_rtjob(p2, 128, 60, 200);
  // stack space 128, wcet 60, deadline is 110 millis after posted
  process_create_rtjob(p4, 128, 60, 110);
  return 0;
}

// we expect that process 6 should run to completion before process 5 as it has higher priority.
int test5_setup(){
  lock_acquire(serial_lock);
  Serial.println("test priority queue - p6 priority higher than p5");
  if (serialEventRun) serialEventRun();
  lock_release(serial_lock);
  // stack space 128, wcet 50, deadline is 100 millis after posted
  if(process_create_prio(p5, 128, 128) < 0) {
    return -1;
  }
  if(process_create_prio(p6, 128, 127) < 0) {
    return -1;
  }
  return 0;
}


// we expect that process 6 and 5 should interleave as they have the same priority.
// then process 1 will execute as it has lower priority.
int test6_setup(){
  lock_acquire(serial_lock);
  Serial.println("test priority queue - p6 priority equal to p5, p1 priority lower than p5, p6");
  if (serialEventRun) serialEventRun();
  lock_release(serial_lock);
  // stack space 128, wcet 50, deadline is 100 millis after posted
  if(process_create_prio(p5, 128, 1) < 0) {
    return -1;
  }
  if(process_create_prio(p6, 128, 1) < 0) {
    return -1;
  }
  if(process_create_prio(p1, 128, 128) < 0) {
    return -1;
  }
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
  if(test6_setup() < 0) {
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
