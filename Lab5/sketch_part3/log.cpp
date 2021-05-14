#include <Arduino.h>
extern "C" {
  #include "log.h"
}

void mlog(const char *msg) {
  Serial.println(msg);
}

void dlog(double d) {
  Serial.println(d);
}

void ilog(unsigned int d) {
  Serial.println(d);
}