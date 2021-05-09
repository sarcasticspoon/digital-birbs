#include <Adafruit_SSD1306.h>

//definitions for OLED SPI
#define OLED_CS 13
#define OLED_DC 12
#define OLED_RST 11
#define OLED_SI 10
#define OLED_CLK 9

//definitions for pins
#define pin_a 2
#define pin_b 3

//definitions for physics
#define scale 3
#define xdim 128
#define ydim 64

//global vars
int counter = 128;
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
  // set pin_a and pin_b to input, and use passive pull up
  pinMode(pin_a, INPUT_PULLUP);
  pinMode(pin_b, INPUT_PULLUP);

  //this initializes the display
  disp.begin(SSD1306_SWITCHCAPVCC);
  disp.clearDisplay();
  disp.display();  

  //this initializes the value of lastStateA
  lastStateA = digitalRead(pin_a);
  //this debouncer uses the concept from this source: http://www.technoblogy.com/show?1YHJ
  // which basically samples the other pin whenever a change is detected, thus always sampling
  // from a stable value.
  attachInterrupt(0, debounceB, CHANGE);
  attachInterrupt(1, debounceA, CHANGE);
}

void loop() {
  //main program 

  //this function draws the ball on the screen
  drawBall();

  //we only change the position of the ball if velocity > 0 which is equivalent to if counter != 128.
  if (counter!=128){
    updatePosition();
  }

  //we calculate "velocity" with this function
  velocity = 128-abs(counter-128);

  //we delay by the "velocity" so that when "velocity" is high, ball goes slower, and vice versa.
  delay(velocity);
}

void debounceB(){
  // samples pin_b
  currentStateB = digitalRead(pin_b);
  // for accuracy
  delay(1);
}

void debounceA(){
  //samples pin_a
  currentStateA = digitalRead(pin_a);
  // determine if rotary has changed.
  updateRotary();
  //for accuracy
  delay(1);
}

void updateRotary(){
  // If pin_a has changed, then we can determine the direction of the change by checking if pin_b is the same or different from A.
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
  //draws the ball
  disp.clearDisplay();
  disp.fillCircle(x,y,4,WHITE);
  disp.display();
}

void updatePosition(){
  /* this function does a few things:
      1) It updates the position of the ball as a function of the velocity of the ball, and its direction.
      2) When the ball makes "contact" with a wall, it bounces off the wall at a randomly chosen vector.
  */

 //these functions scale the x and y components of the vector so that the resultant vector is as close to the unit vector as ints will allow. This is to reduce variations in velocity.
  int xnorm = sign(vector[0])*ceil(abs(vector[0])/sqrt(vector[0]*vector[0]+vector[1]*vector[1]));
  int ynorm = sign(vector[1])*ceil(abs(vector[1])/sqrt(vector[0]*vector[0]+vector[1]*vector[1]));
  //this updates the actual position of the ball. The sign of counter determines if the vector will be flipped, and the scale is a general variable that allows us to calibrate speed.
  x = (x+sign(128-counter)*xnorm*scale);
  y = (y+sign(128-counter)*ynorm*scale);
  //this part of the code handles the wall bounce.
  if (x < 4 || x > 124) {
    vector[0] = -sign(vector[0])*random(1,4);
    // x = (x+sign(128-counter)*vector[0]*scale);
  }
  if (y < 4 || y > 60) {
    vector[1] = -sign(vector[1])*random(1,4);
    // y = (y+sign(128-counter)*vector[1]*scale);
  }
}

int sign(int x) {
  // helper function returns the sign of x
    return (x > 0) - (x < 0);
}
