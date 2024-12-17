// #include <Arduino.h>

/* 
The Amanuensis class abstracts away the low level read/write operations that interact with the EEPROM.
*/

class Amanuensis
{
  public:
    Amanuensis();
    void start();
    byte readAddress(int address);
    void writeAddress(int address, byte data);
    static char ADDR[15];
    static char DATA[8];
    static char WE;
    static char OE;
    static char CE;
    static char RED_LED;
    static char GREEN_LED;
  private:
    void setAddress(int address);
    void setData (byte data);
};


Amanuensis::Amanuensis() {
}

// Arduino pin corresponding to each shield pin; follows the 28c256 pinout order; number 0 is used for GND, VCC and first element (to make the array 1-index based).
char arduino_pins_map[] = {0, 22, 24, 26, 28, 34, 36, 38, 40, 42, 46, 48, 50, 52, 0, 53, 51, 49, 47, 43, 41, 39, 37, 35, 29, 27, 25, 23, 0};

// Identifies pins A0 to A15 in order. They are mapped to the corresponding physical Arduino pin.
char Amanuensis::ADDR[15] = {
  arduino_pins_map[10],
  arduino_pins_map[9],
  arduino_pins_map[8],
  arduino_pins_map[7],
  arduino_pins_map[6],
  arduino_pins_map[5],
  arduino_pins_map[4],
  arduino_pins_map[3],
  arduino_pins_map[25],
  arduino_pins_map[24],
  arduino_pins_map[21],
  arduino_pins_map[23],
  arduino_pins_map[2],
  arduino_pins_map[26],
  arduino_pins_map[1]
};

// Identifies pins IO0 to IO7 in order. They are mapped to the corresponding physical Arduino pin.
char Amanuensis::DATA[8] = {
  arduino_pins_map[11],
  arduino_pins_map[12],
  arduino_pins_map[13],
  arduino_pins_map[15],
  arduino_pins_map[16],
  arduino_pins_map[17],
  arduino_pins_map[18],
  arduino_pins_map[19],
};

char Amanuensis::WE = arduino_pins_map[27];
char Amanuensis::OE = arduino_pins_map[22];
char Amanuensis::CE = arduino_pins_map[20];

char Amanuensis::RED_LED = 31;
char Amanuensis::GREEN_LED = 30;


void Amanuensis::start() {

  /*  Bugfix:
      CHIP ENABLE is used to disable the chip on startup and enable it only
      when all the pins have been setup (using pinMode()). Before that,
      the pins may have floating values that may produce undesired reads/writes.
  */
  pinMode(CE, OUTPUT);  // Active on low
  digitalWrite(CE, HIGH);

  /*  Safety consideration:
      If both Arduino and the EEPROM are outputing (low impedance state)
      they could set different voltage levels, which
      results in a short circuit that may damage both.
      As a safety measure, the EEPROM data is always disabled
      (OE=HIGH, high impedance state) until needed, and afterwards disabled again.
  */
  for (int n = 0; n < 15; n += 1) {
    pinMode(ADDR[n], OUTPUT);  // EEPROM addresses are always inputs
  }
  for (int n = 0; n < 8; n += 1) {
    /*  As the write cycle is more time consuming, data pins are by default set as outputs,
        ready to write. If a read cycle is needed, they must be set to inputs first and then reset to outputs.
        Using OE=HIGH to put the EEPROMs DATA pins in high impedance state, avoiding short circuits.
    */
    pinMode(DATA[n], OUTPUT);
  }

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, LOW);

  pinMode(WE, OUTPUT);  // Active on low
  pinMode(OE, OUTPUT);  // Active on low

  // NON-writting mode
  digitalWrite(WE, HIGH);
  // Outputs off (sets chip's DATA off to avoid short-circuit with Arduino)
  digitalWrite(OE, HIGH);  // High impedance outputs, for safety
  // Chip enabled (IMP done last!)
  digitalWrite(CE, LOW);  // High impedance outputs, for safety
}

void Amanuensis::setAddress(int address) {
  /* Address: 0 -to- 32767 */
  for (int j = 0; j < 15; j++) {
    digitalWrite(ADDR[j], address & 1);
    address = address >> 1;
    bool lastBit = (address & 0b00000001) != 0;
    digitalWrite(GREEN_LED, lastBit);
  }

  digitalWrite(GREEN_LED, LOW);
}

void Amanuensis::setData(byte data) {
  // DATA shall be OUTPUTS here
  for (int i=0; i<8; i++) {
    digitalWrite(DATA[i], data & 1);
    data = data >> 1;
    bool lastBit = (data & 0b00000001) != 0;
    digitalWrite(GREEN_LED, lastBit);
  }

  digitalWrite(GREEN_LED, LOW);
}

byte Amanuensis::readAddress(int address) {
  byte data = 0;
  this->setAddress(address);

  for (int n = 0; n < 8; n += 1) {
    pinMode(DATA[n], INPUT);
  }
  digitalWrite(OE, LOW);

  for (int n = 7; n >= 0; n -= 1) {
    data = (data << 1) + digitalRead(DATA[n]);

    bool lastBit = (data & 0b00000001) != 0;
    digitalWrite(GREEN_LED, lastBit);
  }

  digitalWrite(OE, HIGH);
  for (int n = 0; n < 8; n += 1) {
    pinMode(DATA[n], OUTPUT);
  }

  digitalWrite(GREEN_LED, LOW);

  return data;
}

void Amanuensis::writeAddress(int address, byte data) {

  this->setAddress(address);
  this->setData(data);

  digitalWrite(OE, HIGH);
  digitalWrite(CE, LOW);

  digitalWrite(WE, LOW);
  delayMicroseconds(1);
  digitalWrite(WE, HIGH);

  /* Speed improvements can happen here.
     Recommended delay of 10ms, however,
     times down to 7ms were tested succesfully
  */
  delay(7);  // Actual writing time
}

/*
The code below implements the communication with the CLI, written in Python, and interacts with the EEPROM through the Amanuensis class
*/

Amanuensis amanuensis;

unsigned char addressHalvesBuffer[32];  // 2 bytes per item
unsigned int addressBuffer[16];         // 4 bytes per item
unsigned char dataBuffer[16];           // 2 bytes per item
size_t bytesReceived;
char mode;
int write_amount;

unsigned int MAX_ADDRESS = 32767;


void sendData() {
   // send data to CLI
   if (bytesReceived) {
    for(int i=0; i<16; i++) {
    Serial.write(dataBuffer[i]);
    }
  }
}

void combineAddresses(unsigned char *halfAddresses, unsigned int *destination, int len) {
  // 0xab + 0xcd -> 0xabcd ; len=1
  for (int i=0; i<len; i++) {
    destination[i] = halfAddresses[2*i]<<8|halfAddresses[2*i+1];
  }
}

void wait_for_ACK() {
  // Halt operation until ACK received after data sent
  bool ack = false;
  while (!ack) { 
    if (Serial.available()) {
      ack = Serial.read() == 0x06;
      }
    }
}

void wait_until_data_in_buffer() {
  while (!Serial.available()) { }
}

char handshake() {
  wait_until_data_in_buffer();
  
  Serial.readBytes(dataBuffer, 1);
  Serial.write(0x06);  // sends ACK

  mode = dataBuffer[0];
  
  return mode;
}

void execute(char mode) {
  if(mode == 'R') {

    /* Mode: read EEPROM Range of addresses
       Reads a block of 16 addresses and sends them back.
       A single address read is considered a particular case
       of a range read with a range consisting of a single address.
    */
    
    wait_until_data_in_buffer();
    
    Serial.readBytes(addressHalvesBuffer, 4);
    Serial.write(0x06);  // sends ACK

    unsigned int theAddressBeg;
    unsigned int theAddressEnd;
    unsigned char theData;
    
    combineAddresses(&addressHalvesBuffer[0], &theAddressBeg, 1);
    combineAddresses(&addressHalvesBuffer[2], &theAddressEnd, 1);

    for (unsigned int theAddress=theAddressBeg; theAddress<=theAddressEnd; theAddress+=16) {
      for (int i=0; i<16; i++) {
        theData = amanuensis.readAddress(theAddress+i);
        Serial.write(&theData, 1);
      }
      wait_for_ACK();
    }
  }

  else if(mode == 'W') {
    
    /* Mode: Write EEPROM address
       Gets a given address and a value and writes it to the EEPROM.
    */
    
    wait_until_data_in_buffer();
    
    unsigned int theAddress;
    unsigned char theValue;
    unsigned char theData;
    
    Serial.readBytes(addressHalvesBuffer, 2);
    Serial.readBytes(dataBuffer, 1);

    combineAddresses(addressHalvesBuffer, &theAddress, 1);
    theValue = dataBuffer[0];

    theData = amanuensis.readAddress(theAddress);
    if (theData != theValue) {
      amanuensis.writeAddress(theAddress, theValue);
    }

    Serial.write(0x06);  // sends ACK
    
  }

    else if(mode == 'A') {
    
    /* Mode: write All addresses EEPROM
       Overwrite the whole EEPROM with the provided value.
       Since it's a heavy operation, a mode is created for
       it to avoid back and forth communication with the CLI.
    */

    unsigned int theAddress;
    unsigned char theValue;
    unsigned char theData;
    
    wait_until_data_in_buffer();

    Serial.readBytes(dataBuffer, 1);

    theValue = dataBuffer[0];
    
    Serial.write(0x06);  // sends ACK

    int prevProgress = -1;
    int progress = 0;

    for(unsigned int theAddress=0; theAddress<=MAX_ADDRESS; theAddress+=1) {
      theData = amanuensis.readAddress(theAddress);
      if (theData != theValue) {
        amanuensis.writeAddress(theAddress, theValue);
      }

      progress = int(theAddress/(MAX_ADDRESS/100));  // if arithmetic not done in this way, it overfloads the unsigned int and cycles back
      
      if(prevProgress != progress) {
         Serial.write(progress);
         prevProgress = progress;
      }
    }

    Serial.write(0xFF);  // 0xFF: ending charracter to notify CLI
  }
}

void setup() {
  amanuensis.start();
  
  Serial.begin(9600);
  Serial.setTimeout(100);

  Serial.write(0x06);  // sends ACK. Confirm connection
}

void loop() {
  if(Serial.available()) {
    mode = handshake();
    execute(mode);
  }
}
