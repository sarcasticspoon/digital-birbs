#include "testasm.h"

void setup() {
  // put your setup code here, to run once
  Serial.begin(9600);
  Serial.print("Fib(0):");
  Serial.println(fib(0));
  Serial.print("Fib(1):");
  Serial.println(fib(1));
  Serial.print("Fib(2):");
  Serial.println(fib(2));
  Serial.print("Fib(3):");
  Serial.println(fib(3));
  Serial.print("Fib(4):");
  Serial.println(fib(4));
  Serial.print("Fib(5):");
  Serial.println(fib(5));
  Serial.print("Fib(6):");
  Serial.println(fib(6));
  Serial.print("Fib(7):");
  Serial.println(fib(7));
  Serial.print("Fib(8):");
  Serial.println(fib(8));
  Serial.print("Fib(9):");
  Serial.println(fib(9));
  Serial.print("Fib(10):");
  Serial.println(fib(10));
  Serial.print("Fib(11):");
  Serial.println(fib(11));
  Serial.print("Fib(12):");
  Serial.println(fib(12));
  Serial.print("Fib(13):");
  Serial.println(fib(13));
  Serial.print("Fib(14):");
  Serial.println(fib(14));
  Serial.print("Fib(15):");
  Serial.println(fib(15));
  Serial.print("Fib(16):");
  Serial.println(fib(16));
  Serial.print("Fib(17):");
  Serial.println(fib(17));
  pinMode (13, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite (13, HIGH);
  delay(400);
  digitalWrite (13, LOW);
  delay(400);
}
