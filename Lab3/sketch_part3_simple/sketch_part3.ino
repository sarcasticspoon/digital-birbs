#include <Adafruit_SSD1306.h>

#define OLED_CS 13
#define OLED_DC 12
#define OLED_RST 11
#define OLED_SI 10
#define OLED_CLK 9
#define pin_a 2
#define pin_b 3
#define scale 3
#define radius 4

int counter = 128;
int currentStateB;
int currentStateA;
int lastStateA;
int x, y;
// x direction, y direction, can be either -1 or 1;
int vector[2];
String currentDir = "";

Adafruit_SSD1306 disp(128,64,OLED_SI,OLED_CLK,OLED_DC,OLED_RST,OLED_CS);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pin_a, INPUT_PULLUP);
  pinMode(pin_b, INPUT_PULLUP);

  lastStateA = digitalRead(pin_a);
  attachInterrupt(0, debounceB, CHANGE);
  attachInterrupt(1, debounceA, CHANGE);

  // setup oled display
  disp.begin(SSD1306_SWITCHCAPVCC);
  disp.clearDisplay();
  disp.display();
  x = 64;
  y = 32;
  vector[0] = 1;
  vector[1] = 2;
}

void loop() {
  delay(128 - abs(counter - 128));
  if(counter != 128) {
    updatePosition();
  }
  drawBall();
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
      if(counter > 0) {
        counter--;
      }
      currentDir = "CCW";
    } else {
      if(counter < 255) {
        counter++;
      }
      currentDir = "CW";
    }

//    vector[0] = vector[0]*sign(128-counter);
//    vector[1] = vector[1]*sign(128-counter);

    Serial.print("Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter: ");
    Serial.println(counter);
    lastStateA = currentStateA;
  }
}

// draws the ball
void drawBall() {
  disp.clearDisplay();
  disp.fillCircle(x, y, radius, WHITE);
  disp.display();
}

// calculates the next position of the ball
// based on rotary encoder value
void updatePosition() {
  x = (x + sign(128 - counter)*vector[0]*scale);
  y = (y + sign(128 - counter)*vector[1]*scale);
  if(x < 0 + radius || x > 128 - radius) {
    vector[0] = -vector[0];
  }
  if(y < 0 + radius || y > 64 - radius) {
    vector[1] = -vector[1];
  }
}

int sign(int x) {
  return (x > 0) - (x < 0);
}
