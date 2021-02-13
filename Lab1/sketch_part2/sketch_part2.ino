#include "testasm.h"

void setup() {
  // put your setup code here, to run once
  Serial.begin(9600);
  Serial.print("The sum of 128 and 128 is: ");
  Serial.println(testasm(0, 0));
  Serial.print("The diff of 128 and 39 is: ");
  Serial.println(diffval(128, 128));
  pinMode (13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite (13, HIGH);
  delay(400);
  digitalWrite (13, LOW);
  delay(400);
}
