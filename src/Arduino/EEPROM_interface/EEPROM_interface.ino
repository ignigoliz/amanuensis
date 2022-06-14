
/*  The main problem is is reading early, not sending early;
    If you send early, it waits in the buffer; if you read early, you get non-sense.
*/

#include <Amanuensis.h>

Amanuensis amanuensis;

unsigned char addressHalvesBuffer[32];  // 2 bytes
unsigned int addressBuffer[16];         // 4 bytes
unsigned char dataBuffer[16];           // 2 bytes
size_t bytesReceived;
char mode;
int write_amount;

unsigned int MAX_ADDRESS = 32767;


void sendData() {
   // send data to laptop's terminal
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
  // Block until ACK received after data sent
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
    
    /* Mode: Read EEPROM
       Reads a given address and sends its value back
    */
    
    wait_until_data_in_buffer();
    
    Serial.readBytes(addressHalvesBuffer, 2);
    Serial.write(0x06);  // sends ACK

    unsigned int theAddress;
    unsigned char theValue;
    
    combineAddresses(addressHalvesBuffer, &theAddress, 1);
    
    theValue = amanuensis.readAddress(theAddress);
    Serial.write(&theValue, 1);
    wait_for_ACK();

  }

  else if(mode == 'W') {
    
    /* Mode: Write EEPROM
       Gets a given address and a value, and writes it to the EEPROM
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

  else if(mode == 'N') {
    
    /* Mode: read EEPROM raNge
       Reads a block of 16 addresses and sends them back
    */
    
    wait_until_data_in_buffer();
    
    Serial.readBytes(addressHalvesBuffer, 4);
    Serial.write(0x06);  // sends ACK
//    Serial.write(&addressHalvesBuffer[1], 1);

    unsigned int theAddressBeg;
    unsigned int theAddressEnd;
    unsigned char theData;
    
    combineAddresses(&addressHalvesBuffer[0], &theAddressBeg, 1);
    combineAddresses(&addressHalvesBuffer[2], &theAddressEnd, 1);

//    Serial.write(amanuensis.readAddress(theAddressBeg));
//    Serial.write(&theAddressBeg, 2);

    for (unsigned int theAddress=theAddressBeg; theAddress<=theAddressEnd; theAddress+=16) {
      for (int i=0; i<16; i++) {
        theData = amanuensis.readAddress(theAddress+i);
        Serial.write(&theData, 1);
      }
      wait_for_ACK();
    }
  }

    else if(mode == 'A') {
    
    /* Mode: write All addresses EEPROM
       Overwrite the whole EEPROM with the provided value
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

    Serial.write(0xFF);  // Ending charracter to notify laptop's terminal
  }

  /*if(mode == 'W') {
    // MODE: Write file to EEPROM
    int nPackets = write_amount / 16 + 1;
    int sizeLastPacket = write_amount % 16;
    char sizePacket;
    while(nPackets>0) {
      if(nPackets>1) {
        sizePacket = 16;
      }
      else {
        sizePacket = sizeLastPacket;
      }
      if(Serial.available()) {
        Serial.readBytes(dataBuffer, sizePacket);
        Serial.readBytes(addressHalvesBuffer, sizePacket*2);

        nPackets--;

        combineAddresses(sizePacket);

        for (int i=0; i< sizePacket; i++) {
          amanuensis.writeAddress(addressBuffer[i], dataBuffer[i]);
        }
        
        Serial.write(0x06);  // sends ACK  

        for (int i=0; i<sizePacket*2; i++) {
          Serial.write(addressHalvesBuffer[i]);
        }

      }
    }
  }*/
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
