#define TIMER1_INTERVAL_MS 100L
#define USE_TIMER_1 true
#define USE_TIMER_2 false
#define USE_TIMER_3 false
#define USE_TIMER_4 false
#define USE_TIMER_5 false

#include "TimerInterrupt.h"
#include "concurrency_func.h"

lock_t *serial_lock;


void p1(){
  lock_acquire(serial_lock);
  for(int i = 0; i < 50; i++){
    delay(1);
    Serial.print("P1:");
    Serial.println(i);
  }
  lock_release(serial_lock);

  for(int i = 51; i < 100; i++){
    delay(1);
    lock_acquire(serial_lock);
    Serial.print("P1:");
    Serial.println(i);
    lock_release(serial_lock);
  }
  return;
}

void p2(){
  for(int i = 0; i < 100; i++){
    delay(2);
    lock_acquire(serial_lock);
    Serial.print("P2:");
    Serial.println(i);
    lock_release(serial_lock);
  }
  return;
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
  
  lock_init(serial_lock);
  
//  ITimer1.init();
//  if(ITimer1.attachInterruptInterval(TIMER1_INTERVAL_MS, TimerHandler1)) {
//    Serial.println("attach successful");
//  } else {
//    Serial.println("attach not successful");
//  }
}

void loop() {
  process_start();
  while(1) {
    lock_acquire(serial_lock);
    Serial.println("spinning");
    lock_release(serial_lock);
    delay(1000);
  }
}
