
/**
 * 
 * GPLv3 license (read the LICENSE file)
 * 
 * (C) 2016 CalliperHack Team https://github.com/CalliperHack
 * 
 */


// milliseconds between packets
#define MS_BETWEEN_PACKETS 100

#define PIN_CLK 2
#define PIN_SDA 3

#define PACKET_LEN (24 + 24 + 1)
#define PACKET_BUFFER_LEN (PACKET_LEN * 2)

// shared variables ISR <---> program
volatile int packet_bits[PACKET_BUFFER_LEN];
volatile int start_index = -1;
volatile bool new_packet = false;

// interrupt service routine
void isr_clock() {

  // persisted variables
  static int index = 0;
  static unsigned long previous_time = micros();

  // read data signals (SDA) and store them 
  packet_bits[index] = digitalRead(PIN_SDA);

  // detect a new packet
  unsigned long time = micros();
  if (time - previous_time > MS_BETWEEN_PACKETS * 1000 / 2) {

    // new packet
    start_index = index;
    new_packet = true;
  }

  // prepare next cycle
  ++index %= PACKET_BUFFER_LEN;
  previous_time = time;
}



void setup() {

  Serial.begin(115200);
  
  pinMode(PIN_CLK, INPUT_PULLUP);
  pinMode(PIN_SDA, INPUT_PULLUP);

  Serial.println("Reading...");

  // execute isr_clock() when clock signal (CLK) rises
  attachInterrupt(digitalPinToInterrupt(PIN_CLK), isr_clock, RISING);
}


// dump the packet to a human readable string
void dumpBin(int index, bool high, char* prefix) {

  static char bit_string[PACKET_LEN + 1];

  for (int b = 0; b < PACKET_LEN; b++) {
    index %= PACKET_LEN;
    bit_string[b] = (packet_bits[index++] == (high ? HIGH : LOW) ? '1' : '0');
  }    
  bit_string[PACKET_LEN] = '\0';

  // show the info
  Serial.print(prefix);
  Serial.println(bit_string);
}


// decode a BCD digit
int bcd(int index, bool high) {

  int decimal = 0;

  for (int b = 0; b < 4; b++) {    
    if (packet_bits[index++] == (high ? HIGH : LOW)) decimal |= 1;
    decimal <<= 1;
  }

  return (decimal >= 0 && decimal <= 9 ? decimal : -1);
}


// dump the packet to a human readable string
void dumpBcd(int index, int offset, bool high, char* prefix) {

  // string buffer
  static char bit_string[12 + 1];

  // length depends on the offset over the bit stream
  int last = PACKET_LEN - offset;

  // binary to decimal
  int b = offset, c = 0;
  while (last - b >= 4) {

      // decode 4 BCD bits from bit b
      int dec = bcd(b, high);

      // ASCII representation (0-9) or none (_)
      bit_string[c] = (dec < 0 ? '_' : '0' + dec);

      // BCD is 4 bits long
      b += 4;
  }

  // show the info
  Serial.print(prefix);
  Serial.println(bit_string);
}


void loop() {

  if (new_packet && start_index >= 0) {

    // get the previous packet in the buffer
    int index = (start_index + PACKET_LEN);

    // dump the packet to a human readable string
    dumpBin(index, true, "BIN1: ");
    dumpBin(index, false, "BIN0: ");

    // BDC
    dumpBcd(index, 0, true, "BCD01: ");
    dumpBcd(index, 0, false, "BCD00: ");
    dumpBcd(index, 1, true, "BCD11: ");
    dumpBcd(index, 1, false, "BCD10: ");
    dumpBcd(index, 2, true, "BCD21: ");
    dumpBcd(index, 2, false, "BCD20: ");
    dumpBcd(index, 3, true, "BCD31: ");
    dumpBcd(index, 3, false, "BCD30: ");

    // packet is done
    new_packet = false;
  }

}



