#define pin_a 3
#define pin_b 2

int counter = 0;
int currentStateB;
int currentStateA;
int lastStateA;
String currentDir = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pin_a, INPUT_PULLUP);
  pinMode(pin_b, INPUT_PULLUP);

  lastStateA = digitalRead(pin_a);
  attachInterrupt(0, debounceB, CHANGE);
  attachInterrupt(1, debounceA, CHANGE);
}

void loop() {
//  currentStateA = digitalRead(pin_a);
//  Serial.print(digitalRead(pin_b));
//  Serial.println(currentStateA);
//  if (currentStateA != lastStateA && currentStateA == 1) {
//    if (digitalRead(pin_b) != currentStateA) {
//      counter--;
//      currentDir = "CCW";
//    } else {
//      counter++;
//      currentDir = "CW";
//    }
//
//    Serial.print("Direction: ");
//    Serial.print(currentDir);
//    Serial.print(" | Counter: ");
//    Serial.println(counter);
//
//    lastStateA = currentStateA;
//  }
  delay(100);
}

void debounceB(){
  currentStateB = digitalRead(pin_b);
  delay(1);
}

void debounceA(){
  currentStateA = digitalRead(pin_a);
  updateRotary();
  delay(1);
}

void updateRotary(){
  if (currentStateA != lastStateA) {
    if (currentStateB == currentStateA) {
      counter--;
      currentDir = "CCW";
    } else {
      counter++;
      currentDir = "CW";
    }

    Serial.print("Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter: ");
    Serial.println(counter);
    lastStateA = currentStateA;
  }
}
