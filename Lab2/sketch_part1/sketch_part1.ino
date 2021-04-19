const int DIN_PIN = 2;
int last_value = 9;
int last_stable = 9;
unsigned long last_debounce_time = 0;
unsigned long debounce_delay = 50;

void setup() {
  // put your setup code here, to run once:
  // if using Arduino helper code:
// pinMode ( DIN_PIN, INPUT_PULLUP );
  DDRD &= ~(1 << DIN_PIN); // want to set DIN_PIN to receive input
  PORTD |= (1 << DIN_PIN); // want to set DIN_PIN to passive pullup
  Serial.begin( 9600 );
}

void loop() {
  // put your main code here, to run repeatedly:
  int value;
  value = ( PIND & 0b00000100 ) >> 2; //read the pin

  if ( value != last_value ) { 
    last_debounce_time = millis(); 
    }

  if ( value != last_stable && millis() - last_debounce_time > debounce_delay) { //we only want to register a change in the button if the value is stable and the debounce time is greater than the debounce delay
    last_stable = value;
    if ( value == 1 ) {
      Serial.println( "The switch is open." );
    }
    else {
      Serial.println( "The switch is closed." );
    }
  }

  last_value = value;
}
