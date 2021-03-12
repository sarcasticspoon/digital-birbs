int DATA_PIN = 13; // the pins we use to drive the shift registers
int LATCH_PIN = 12;
int CLOCK_PIN = 11;

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
// if using Arduino helper functions:
//  pinMode(LATCH_PIN, OUTPUT);
//  pinMode(DATA_PIN, OUTPUT);
//  pinMode(CLOCK_PIN, OUTPUT);
  DDRB |= 0b111000;
}

void loop() { // want to iteratively go through each column, and set the right cells in each row to light up.
  for (int j = 0; j < 10; j++) {
    for (int k = 0; k < 1000; k++) {
      for(int i = 0; i < 5; i++) {
        PORTB &= 0b01111; // write Latch Pin LOW - digitalWrite(LATCH_PIN, LOW);
        shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, ~(1 << i)); //choose the column
        shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, numbers[j][i]); //choose the row
        PORTB |= 0b10000; // write Latch Pin High - digitalWrite(LATCH_PIN, HIGH); 
      }
    }
  }
}
