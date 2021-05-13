#include <Arduino.h>
extern "C" {
  #include "log.h"
}

void mlog(const char *msg) {
  Serial.println(msg);
}