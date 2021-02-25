#include "testasm.h"

void setup() {
  // put your setup code here, to run once
  Serial.begin(9600);
  Serial.print("Zeroes: sum_and_diff(0, 0): ");
  Serial.println(sum_and_diff(0, 0));
  Serial.print("a>b: sum_and_diff(1, 0): ");
  Serial.println(sum_and_diff(1, 0));
  Serial.print("b>a: sum_and_diff(0, 1): ");
  Serial.println(sum_and_diff(0, 1));
  Serial.print("b>a, 8+ bit value: sum_and_diff(250, 90): ");
  Serial.println(sum_and_diff(250, 90));
  Serial.print("a=b: sum_and_diff(169, 169): ");
  Serial.println(sum_and_diff(169, 169));
  Serial.print("8-bit summands: sum_and_diff(255, 255): ");
  Serial.println(sum_and_diff(255, 255));
  pinMode (13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite (13, HIGH);
  delay(400);
  digitalWrite (13, LOW);
  delay(400);
}
