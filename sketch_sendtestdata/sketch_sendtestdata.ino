#define LITTLE 6856
#define MID 10142
#define BIG 1306

int readDigitalInPin = 5;
int resetDigitalInPin = 7;

unsigned int fishLenghts[] = {LITTLE, MID, BIG, 1000, 50};
unsigned int fishClass = 0;

int status_read;
int status_reset;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(115200);
  pinMode(readDigitalInPin, INPUT_PULLUP);
  pinMode(resetDigitalInPin, INPUT_PULLUP);
  
  status_read = digitalRead(readDigitalInPin);
  status_reset = digitalRead(resetDigitalInPin);
  
}



void loop() {

  fishClass = fishClass % (sizeof(fishLenghts) / sizeof(int));  
  
  if (digitalRead(readDigitalInPin) ^ status_read)
  {
    if (status_read == HIGH) {
        // read was pressed
        Serial.println("BTN A ");
        fishClass++;
    }
    
    status_read = !status_read;
  }
  
  if (digitalRead(resetDigitalInPin) ^ status_reset)
  {
    if (status_reset == HIGH) {
        // reset was pressed
        Serial.println("RESET");
        fishClass++;
    }
    
    status_reset = !status_reset;
  }
  
  Serial.print("M ");
  Serial.println(fishLenghts[fishClass]);

  delay(150);
  
  
  /**
  
  static int fishClass = 0;
  while (fishClass < 3) {
    while( digitalRead(readDigitalInPin) == HIGH ) {
      
      if (digitalRead(resetDigitalInPin) == LOW) {
        Serial.println('reset!!!!');
        break;
      };
      
      Serial.println( fishLenghts[fishClass] );
    };
    
    fishClass++;
    
  }
  
  */
  
  
}
