#define trigger_pin 13
#define echo_pin 12
#define red_pin 11
#define green_pin 10
#define pc_pin 0
int pc_reading;
int red_value, green_value;
int max_brightness;
float duration, distance, percentage;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
}

void loop() {
  // set the trigger pin to low, just to make sure
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);
  // set it to high to send out 8 cycle sonic burst
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  // turn to low again
  digitalWrite(trigger_pin, LOW);
  // take in input from echo pin, start when high, end when low
  duration = pulseIn(echo_pin, HIGH);
  // calculate distance from duration
  distance = (duration*0.0343)/2;
  Serial.print("Distance is: ");
  Serial.print(distance);

  // get brightness from photocell
  pc_reading = analogRead(pc_pin);
  Serial.print("     Brightness is: ");
  Serial.println(pc_reading);
  
  max_brightness = map(pc_reading, 0, 1023, 0, 255);
  red_value = map(distance, 0, 50, 0, max_brightness);
  green_value = max_brightness - red_value;
  
  analogWrite(red_pin, red_value);
  analogWrite(green_pin, green_value);
  
  delay(100);
}
