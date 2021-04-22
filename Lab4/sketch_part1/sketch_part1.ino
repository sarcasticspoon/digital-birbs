#define TIMER1_INTERVAL_MS 100L
#define USE_TIMER_1 true
#define USE_TIMER_2 false
#define USE_TIMER_3 false
#define USE_TIMER_4 false
#define USE_TIMER_5 false

#include "TimerInterrupt.h"
#include "concurrency_func.h"

#define LED1 10
#define LED2 11

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

int test1_setup() {
  Serial.println("Testing 1 process only P1");
  if(process_create(p1, 64) < 0) {
    return -1;
  }
  return 0;
}

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

//void TimerHandler1(){
//  Serial.println("context switch");
//  yield();
//}

void setup() {
  Serial.begin(9600);
  if(process_create(p1, 64) < 0) {
    return;
  }
  if(process_create(p2, 64) < 0) {
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
