#include <Arduino.h>
extern "C" {
  #include "log.h"
}

void mlog(const char *msg) {
  Serial.print(msg);
}

void dlog(double d) {
  Serial.print(d);
}

void ilog(unsigned int d) {
  Serial.print(d);
}