#include "concurrency_func.h"
#define blue_pin 11
#define red_pin 10

lock_t *serial_lock;

void blue(){
  for(int i = 0; i < 50; i++){
    lock_acquire(serial_lock);
    analogWrite(blue_pin, 255);
    delay(100);
    analogWrite(blue_pin, 0);
    delay(50);
    lock_release(serial_lock);
  }
  return;
}

void red(){
  for(int i = 0; i < 50; i++){
    lock_acquire(serial_lock);
    analogWrite(red_pin, 255);
    delay(100);
    analogWrite(red_pin, 0);
    delay(50);
    lock_release(serial_lock);
  }
  return;
}

void testlight_setup() {
  Serial.begin(9600);
  if(process_create(blue, 64) < 0) {
    return;
  }
  if(process_create(red, 64) < 0) {
    return;
  }
  pinMode(blue_pin, OUTPUT);
  pinMode(red_pin, OUTPUT);
}

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
