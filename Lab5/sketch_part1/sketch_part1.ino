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
  return;
}

void p3(){
  for(int i = 0; i < 50; i++){
    delay(4);
    Serial.print("P3:");
    Serial.println(i);
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
 * P5 and P6 each turn a LED on and off (blinking) in a loop
 */
void p5(){
  // turn on an LED
  for(int i = 0; i < 20; i++) {
    digitalWrite(LED1, HIGH);
    delay(100);
    digitalWrite(LED1, LOW);
    delay(100);
  }
  return;
}

void p6(){
  // turn on another LED
  for(int i = 0; i < 20; i++) {
    digitalWrite(LED2, HIGH);
    delay(100);
    digitalWrite(LED2, LOW);
    delay(100);
  }
  return;
}

/* 
 *  we expect to see P1 print 1 - 100, then terminate
 *  the main process will then print 'spinning' continuously
 *  this shows that one process will continue to run by itself even if interrupted
*/
int test1_setup() {
  Serial.println("Testing 1 process only P1");
  if(process_create(p1, 64) < 0) {
    return -1;
  }
  return 0;
}

/*
 * here we test 2 concurrent processes with different delays
 * P2 has a delay of 2 ms, loops 100 times
 * P3 has a delay of 4 ms, loops 50 times
 * we expect to see P2 to print twice as many lines as P3 in a time slice
 * ex: P2:1 P2:2 P2:3 P2:4 P3:1 P3:2 P2:5 ...
 * both should interleave the entire time and terminate at around the same time
 * after both have terminated, the main process will print 'spinning'
 */
int test2_setup(){
  Serial.println("Testing 2 processes with different delays. P2 P3");
  if(process_create(p2, 64) < 0) {
    return -1;
  }
  if(process_create(p3, 64) < 0) {
    return -1;
  }
  return 0;
}

/*
 * we test 2 concurrent processes that run for different amounts of time
 * P3 and P4 each run for 50 iterations, but have different delays
 * we expect to see P3 and P4 interleave at first, with P4 printing more than P3
 * then P4 terminates, and P3 continues by itself
 * after P3 terminates, the main process will print 'spinning'
 */
int test3_setup() {
  Serial.println("Testing 2 processes, 1 finishes first. P3 P4");
  if(process_create(p3, 64) < 0) {
    return -1;
  }
  if(process_create(p4, 64) < 0) {
    return -1;
  }
  return 0;
}

/*
 * we test all 4 concurrent processes at the same time
 * all four processes should interleave at the beginning
 * P4 will terminate first, then P1 will terminate
 * P2 and P3 will interleave until both terminate at around the same time
 * the main process will print 'spinning'
 */
int test4_setup(){
  Serial.println("Testing 4 processes. P1 P2 P3 P4");
  if(process_create(p1, 64) < 0) {
    return -1;
  }
  if(process_create(p2, 64) < 0) {
    return -1;
  }
  if(process_create(p3, 64) < 0) {
    return -1;
  }
  if(process_create(p4, 64) < 0) {
    return -1;
  }
  return 0;
}

/*
 * we visually test the concurrency by having two processes flash separate LEDs
 * we expect to see one LED flash, then turn off as the other LED flashes
 * the LEDs should alternate flashing until both processes terminate at the same time
 * the main process will print 'spinning'
 */
int test5_setup(){
  Serial.println("Visual test. Turn on and off LEDS. P5 P6");
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  if(process_create(p5, 64) < 0) {
    return -1;
  }
  if(process_create(p6, 64) < 0) {
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
    delay(10);
  }
}
