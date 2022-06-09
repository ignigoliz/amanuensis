/*
  Amanuensis.h - Library for flashing to EEPROM
  Created by Iñigo Lariz, February, 2022.
*/

#ifndef AMANUENSIS_H
#define AMANUENSIS_H

#include "Arduino.h"

class Amanuensis
{
  public:
    Amanuensis();
    void start();
    byte readAddress(int address);
    void writeAddress(int address, byte data);
    static char ADDR[15];
    static char DATA[8];
    static int WE;
    static int OE;
    static int CE;
  private:
    void setAddress(int address);
    void setData (byte data);
};

#endif
