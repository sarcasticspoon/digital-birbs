int pc_pin = 0; //a0
int led_pin = 11; //pd11, pwm
int pc_reading;
int led_brightness;

void setup() {
  // enable printing to serial monitor
  Serial.begin(9600);
}

void loop() {
  // read in value from photocell
  /*  TO IMPLEMENT analogRead(), we need to set tell the ADC to read the voltage from our analogy pin,
      and get back the 10 bits that it sets after doing the analog to digital conversion. The details
      of the code look like this:

      REF: https://microelex.blogspot.com/p/1.html
      REF: https://www.instructables.com/Introduction-to-ADC-in-AVR-Microcontroller-for-Beg/
      REF: https://www.gammon.com.au/adc

      setup() {
        ADCSRA = bit(7); // turn ADC on
        ADCSRA != bit (0) | bit(1) | bit(2); //set prescalar to 128
        ADMUX = bit(1) | (pc_pin & 0x07); //set the reference voltage and set the multiplexer pin
        bool busy;
      }

      loop() {
        if (!busy) {
          bitSet (ADCSRA, 6);//sample the voltage
          busy = true;
        }

        if (ADCSRA & bit(6) == bit(6)) {
          int value = ADC; //read result from ADCL and ADCH
          busy = false;
          //do something with value here.
        }
      }
  */
  pc_reading = analogRead(pc_pin);
  // map to 0-255 range
  led_brightness = map(pc_reading, 0, 1023, 0, 255);
  // print the current value
  Serial.print("The brightness of the LED is: ");
  Serial.println(led_brightness);
  // set the brightness of the RGB LED, only using blue channel
  analogWrite(led_pin, led_brightness);
  // wait 1 second
  delay(1000);
}
