int pc_pin = 0; //a0
int led_pin = 11; //pd11, pwm
int pc_reading;
int led_brightness;

void setup() {
  Serial.begin(9600);
}

void loop() {
  // read in pin from photocell
  pc_reading = analogRead(pc_pin);
  // map to 0-255 range
  led_brightness = map(pc_reading, 0, 1023, 0, 255);
  // print the current value
  Serial.println("The brightness of the LED is: ");
  Serial.print(led_brightness);
  // set the brightness of the RGB LED, only using blue channel
  analogWrite(led_pin, led_brightness);
  // wait 1 second
  delay(1000);
}
