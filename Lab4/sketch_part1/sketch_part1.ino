#include "concurrency_func.h"
#define USE_TIMER_0 true
#define TIMER0_INTERVAL_MS 50L

void p1(){
  return;
}

void p2(){
  return;
}

void timerHandler0(){
  yield();
}

void setup() {
  if(process_create(p1, 64) < 0) {
    return;
  }
  if(process_create(p2, 64) < 0) {
    return;
  }
  ITimer0.init();
  Serial.begin(9600);
}

void loop() {
  process_start();
  while(1);
}
