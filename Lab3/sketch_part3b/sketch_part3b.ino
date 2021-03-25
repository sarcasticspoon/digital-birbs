#include <Adafruit_SSD1306.h>

#define OLED_CS 13
#define OLED_DC 12
#define OLED_RST 11
#define OLED_SI 10
#define OLED_CLK 9

#define pin_a 2
#define pin_b 3
#define scale 3
#define xdim 128
#define ydim 64

int counter = 255;
int currentStateB;
int currentStateA;
int lastStateA;
int x = xdim/2;
int y = ydim/2;
int vector[2] = {1, 2};
int velocity = 0;
String currentDir = "";

Adafruit_SSD1306 disp(xdim,ydim,OLED_SI,OLED_CLK,OLED_DC,OLED_RST,OLED_CS);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pin_a, INPUT_PULLUP);
  pinMode(pin_b, INPUT_PULLUP);

  disp.begin(SSD1306_SWITCHCAPVCC);
  disp.clearDisplay();
  disp.display();  

  lastStateA = digitalRead(pin_a);
  attachInterrupt(0, debounceB, CHANGE);
  attachInterrupt(1, debounceA, CHANGE);
}

void loop() {
  drawBall();
  if (counter!=128){
    updatePosition();
  }
  velocity = 128-abs(counter-128);
  delay(velocity);
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
      if (counter > 0){
        counter--;
      }
      currentDir = "CCW";
    } else {
      if (counter < 255){
        counter++;
      }
      currentDir = "CW";
    }
    Serial.print("x: ");
    Serial.print(vector[0]);
    Serial.print("y: ");
    Serial.print(vector[1]);
    Serial.print(" | Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter: ");
    Serial.println(counter);
    lastStateA = currentStateA;
  }
}

void drawBall(){
  
  disp.clearDisplay();
  disp.fillCircle(x,y,4,WHITE);
  disp.display();
}

void updatePosition(){
  int xnorm = sign(vector[0])*ceil(abs(vector[0])/sqrt(vector[0]*vector[0]+vector[1]*vector[1]));
  int ynorm = sign(vector[1])*ceil(abs(vector[1])/sqrt(vector[0]*vector[0]+vector[1]*vector[1]));
  x = (x+sign(128-counter)*xnorm*scale);
  y = (y+sign(128-counter)*ynorm*scale);
  if (x < 4 || x > 124) {
    vector[0] = -sign(vector[0])*random(1,4);
    x = (x+sign(128-counter)*vector[0]*scale);
  }
  if (y < 4 || y > 60) {
    vector[1] = -sign(vector[1])*random(1,4);
    y = (y+sign(128-counter)*vector[1]*scale);
  }
}

int sign(int x) {
    return (x > 0) - (x < 0);
}
