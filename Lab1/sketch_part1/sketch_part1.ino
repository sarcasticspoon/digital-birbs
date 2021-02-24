#include "testasm.h"

void setup() {
  // put your setup code here, to run once
  Serial.begin(9600);
  Serial.print("Zeroes: 0+0=");
  Serial.println(testasm(0, 0));
  Serial.print("<8 bit sum: 43+21=");
  Serial.println(diffval(43, 21));
  Serial.print(">=16 bit sum: 169+169=");
  Serial.println(diffval(169, 169));
  Serial.print("8 bit summands: 255+255=");
  Serial.println(diffval(255, 255));
  pinMode (13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite (13, HIGH);
  delay(400);
  digitalWrite (13, LOW);
  delay(400);
}
