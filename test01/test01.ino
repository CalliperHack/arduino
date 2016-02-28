
/**
 * 
 * GPLv3 license (see README.md)
 * 
 * (C) 2016 CalliperHack Team https://github.com/CalliperHack
 * 
 */


// minimum time in microseconds to detect new packets
#define MICROSECONDS_BETWEEN_PACKETS 50000

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

  static int index = 0;
  static unsigned long previous_time = micros();

  // read data signals (SDA) and store them 
  packet_bits[index++] = digitalRead(PIN_SDA);

  // array bounds
  index %= PACKET_BUFFER_LEN;

  unsigned long time = micros();

  // detect a new packet
  if (time - previous_time > MICROSECONDS_BETWEEN_PACKETS) {

    // new packet
    previous_time = time;
    start_index = index;
    new_packet = true;
  }
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
void dump1(int index) {

  static char bit_string[] = "0000000000000000000000000000000000000000000000000";

  for (int b = 0; b < PACKET_LEN; b++) {
    index %= PACKET_LEN;
    bit_string[b] = (packet_bits[index++] == HIGH ? '1' : '0');
  }    

  // show the info
  Serial.println(bit_string);
}


void loop() {

  if (new_packet) {

    // get the previous packet in the buffer
    int index = (start_index + PACKET_LEN);

    // dump the packet to a human readable string
    dump1(index);

    // packet is done
    new_packet = false;
  }

}



