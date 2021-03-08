const int DATA_PIN = 13;
const int LATCH_PIN = 12;
const int CLOCK_PIN = 11;
const int BUTTON_PIN = 2;
int digit = 0;
volatile unsigned long last_time = 0;
const unsigned long debounce_time = 5;
int last_state = 1;
int last_stable = 1;
bool interrupt_flag = false;

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
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), debounce_increment, CHANGE);
}

void loop() {
  // scan through columns to show number specified by global var digit
  for(int i = 0; i < 5; i++) {
    digitalWrite(LATCH_PIN, LOW);
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ~(1 << i));
    shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, numbers[digit][i]);
    digitalWrite(LATCH_PIN, HIGH);
  }
  if(interrupt_flag) {
    increment();
    interrupt_flag = false;
  }
}

void debounce_increment() {
  int value = digitalRead(BUTTON_PIN);
  if(value != last_stable && last_stable == last_state) {
    last_time = millis();
    last_state = value;
    return;
  }

  if(value != last_stable && value == last_state) {
    if (millis() - last_time > debounce_time) {
      interrupt_flag = true;
      last_stable = value;
    }
  }
  //  volatile unsigned long curr = millis();
//  if(curr - last_time > debounce_time) {
//    interrupt_flag = true;
//    last_time = curr;
//  }
}

void increment() {
  digit = (digit + 1) % 10;
}
