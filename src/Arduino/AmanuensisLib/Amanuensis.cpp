#include "Amanuensis.h"
#include "Arduino.h"
// Knows how to interact (read, write) with the EEPROM (65C256)

Amanuensis::Amanuensis() {
}

// Ordered, so that the first element matches A0 and D0
char Amanuensis::ADDR[] = {47, 45, 51, 50, 44, 46, 48, 40, 24, 26, 53, 28, 38, 22, 36};
char Amanuensis::DATA[] = {49, 41, 39, 35, 33, 23, 25, 27};
int Amanuensis::WE = 32;
int Amanuensis::OE = 52;
int Amanuensis::CE = 29;
unsigned int MAX_ADDRESS = 32767;

void Amanuensis::start() {

  /*  Bug: the routine of calling pinMode on CE, WE and OE sets to undetermined values
      the address that is set before these. To avoid this, chip is deactivated on
      startup, and gets activated when all the pins have been setup.
  */
  pinMode(CE, OUTPUT);  // Active on low
  digitalWrite(CE, HIGH);

  /*  Safety consideration:
      If both Arduino and the EEPROM are outputing (low impedance state)
      they could set different voltage levels, which
      results in a short circuit that may damage both.
      As a safety measure, the EEPROM data is disabled
      (OE=HIGH, high impedance state) until needed.
  */
  for (int n = 0; n < 15; n += 1) {
    pinMode(ADDR[n], OUTPUT);  // EEPROM addresses are always inputs
  }
  for (int n = 0; n < 8; n += 1) {
    /*  As the write cycle is more time consuming, pins are by default ready to write
        During read cycle their behaviour is being set as Inputs and then reset to o
        utputs to avoid short circuits, use OE=HIGH to put EEPROMs DATA in high
        impedance (Input)
    */
    pinMode(DATA[n], OUTPUT);
  }

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
  }
}

void Amanuensis::setData (byte data) {
  // DATA shall be OUTPUTS here
  for (int i=0; i<8; i++) {
    digitalWrite(DATA[i], data & 1);
    data = data >> 1;
  }
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
  }

  digitalWrite(OE, HIGH);
  for (int n = 0; n < 8; n += 1) {
    pinMode(DATA[n], OUTPUT);
  }

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
