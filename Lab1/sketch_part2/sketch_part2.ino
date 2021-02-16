#include "testasm.h"

void setup() {
  // put your setup code here, to run once
  Serial.begin(9600);
  Serial.print("The sum of 128 and 255 is: ");
  Serial.println(sum_and_diff(128, 255));
  Serial.print("The sum of 128 and 128 is: ");
  Serial.println(sum_and_diff(128, 128));
  Serial.print("The sum of 255 and 255 is: ");
  Serial.println(sum_and_diff(255, 255));
  Serial.print("The sum of 0 and 0 is: ");
  Serial.println(sum_and_diff(0, 0));
  Serial.print("The sum of 1 and 0 is: ");
  Serial.println(sum_and_diff(1, 0));
  Serial.print("The sum of 0 and 1 is: ");
  Serial.println(sum_and_diff(0, 1));
  pinMode (13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite (13, HIGH);
  delay(400);
  digitalWrite (13, LOW);
  delay(400);
}
