const int DIN_PIN = 2;
int last_value = 9;
int last_stable = 9;
unsigned long last_debounce_time = 0;
unsigned long debounce_delay = 50;

//void setPin( char pin, int setting, int pullUp ) {
//  if ( pullUp == 1 ) {
//    PORTD |= ( pullUp << pin );
//  } else {
////    char temp = ~PORTD;
////    temp |= ( pullUp << pin );
////    PORTD = ~temp;
//  }
//
//  DDRD &= ~( !setting << pin );
//  
//}

void setup() {
  // put your setup code here, to run once:
  DDRD &= ~(1 << DIN_PIN);
  PORTD |= (1 << DIN_PIN);
//  pinMode ( DIN_PIN, INPUT_PULLUP );
  Serial.begin( 9600 );
}

void loop() {
  // put your main code here, to run repeatedly:
  int value;
  value = ( PIND & 0b00000100 ) >> 2;

  if ( value != last_value ) { 
    last_debounce_time = millis(); 
    }

  if ( value != last_stable && millis() - last_debounce_time > debounce_delay) {
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
