int latchPin = 12;
int clockPin = 11;
int dataPin = 13;

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
  pinMode( latchPin, OUTPUT );
  pinMode( dataPin, OUTPUT );
  pinMode( clockPin, OUTPUT );
  Serial.begin( 9600 );
}

void loop() {
  //count up routine
  for (int j = 0; j < 256; j++) {
    //ground latchPin and hold low for as long as you are transmitting
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, LSBFIRST, 0b0000000);
    //return the latch pin high to signal chip that it
    //no longer needs to listen for information
    digitalWrite(latchPin, HIGH);
    delay(1000);
  }
}
