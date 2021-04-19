#define TIMER1_INTERVAL_MS 50L
#define USE_TIMER_1 true
#define USE_TIMER_2 false
#define USE_TIMER_3 false
#define USE_TIMER_4 false
#define USE_TIMER_5 false

#include "TimerInterrupt.h"
#include "concurrency_func.h"

void p1(){
  for(int i = 0; i < 100; i++){
    delay(10);
    Serial.print("P1:");
    Serial.println(i);
  }
  return;
}

void p2(){
  for(int i = 0; i < 100; i++){
    delay(10);
    Serial.print("P2:");
    Serial.println(i);
  }
  return;
}

void TimerHandler1(){
  Serial.println("context switch");
  yield();
}

void setup() {
  Serial.begin(9600);
  if(process_create(p1, 64) < 0) {
    return;
  }
  if(process_create(p2, 64) < 0) {
    return;
  }
  ITimer1.init();
  if(ITimer1.attachInterruptInterval(TIMER1_INTERVAL_MS, TimerHandler1)) {
    Serial.println("attach successful");
  } else {
    Serial.println("attach not successful");
  }
}

void loop() {
  process_start();
  while(1);
}
