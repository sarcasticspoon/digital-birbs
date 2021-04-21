#include "concurrency_func.h"

lock_t *serial_lock;

void p2(){
  lock_acquire(serial_lock);
  for(int i = 0; i < 50; i++){
    delay(1);
    Serial.print("P2:");
    Serial.println(i);
    }
  lock_release(serial_lock);

  for(int i = 51; i < 100; i++){
    delay(1);
    lock_acquire(serial_lock);
    Serial.print("P2:");
    Serial.println(i);
    lock_release(serial_lock);
  }
  return;
}

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

void setup() {
  Serial.begin(9600);
  if(process_create(p1, 64) < 0) {
    return;
  }
  if(process_create(p2, 64) < 0) {
    return;
  }

  serial_lock = (lock_t*)malloc(sizeof(lock_t));
  lock_init(serial_lock);
  if(!serial_lock->lock) {Serial.println("lock successfully initialized");}
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
