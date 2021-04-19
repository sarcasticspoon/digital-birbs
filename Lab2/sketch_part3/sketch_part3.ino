const int DATA_PIN = 13;
const int LATCH_PIN = 12;
const int CLOCK_PIN = 11;
const int BUTTON_PIN = 2;
int digit = 0;
volatile unsigned long last_time = 0;
const unsigned long debounce_time = 125; //define debounce time here
int last_stable = 1;
bool interrupt_flag = false;
int DELAY = 0;
bool delay_flag = false;

static char numbers[10][5] = {
  {0x3E, 0x51, 0x49, 0x45, 0x3E},// 0
  {0x00, 0x42, 0x7F, 0x40, 0x00},// 1
  {0x42, 0x61, 0x51, 0x49, 0x46},// 2
  {0x21, 0x41, 0x45, 0x4B, 0x31},// 3
  {0x18, 0x14, 0x12, 0x7F, 0x10},// 4
  {0x27, 0x45, 0x45, 0x45, 0x39},// 5
  {0x3C, 0x4A, 0x49, 0x49, 0x30},// 6
  {0x01, 0x71, 0x09, 0x05, 0x03},// 7
  {0x36, 0x49, 0x49, 0x49, 0x36},// 8
  {0x06, 0x49, 0x49, 0x29, 0x1E}// 9
};

void setup() {
  // put your setup code here, to run once:
//  pinMode(LATCH_PIN, OUTPUT);
//  pinMode(DATA_PIN, OUTPUT);
//  pinMode(CLOCK_PIN, OUTPUT);
  DDRB |= 0b111000; // set latch pin, data pin, clock pin to output
  DDRD &= ~(1<< BUTTON_PIN); // set pd2 to input for button
  PORTD |= (1<< BUTTON_PIN); // set passive pullup
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), debounce_increment, CHANGE); //attach our interrupt handler
}

void loop() {
  // scan through columns to show number specified by global var digit
  for(int i = 0; i < 5; i++) {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ~(1 << i)); // select each column
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, numbers[digit][i]); //select each row to display
    digitalWrite(LATCH_PIN, HIGH);
  }
  if(interrupt_flag && !delay_flag) {         // this code increments the global counter after the interrupt flag is set.
    increment();
    last_time = millis();
    DELAY = 250;
    delay_flag = true;
    interrupt_flag = false;
  }
  if(DELAY > 0) { 
    DELAY--;
  }
  if(DELAY == 0 && delay_flag) { // an artificial delay is induced to make debouncing more robust
    delay_flag = false;
  }
  
}

void debounce_increment() { //this is our code to handle the interrupt and also debounce.
  int value = digitalRead(BUTTON_PIN);
 
  if (value != last_stable) {
    if (millis() - last_time > debounce_time) { // last time will have been set only when interrupt is handled, so this guarantees that the first change will be detected, but not subsequent changes.
        if(value == 0) {
          interrupt_flag = true; // only set the flag if the button is depressed
        }
      last_stable = value;
    }
  }
}

void increment() {
  digit = (digit + 1) % 10; // ensures the range of the digit will be between 0 - 9
}
