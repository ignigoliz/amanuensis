# Amanuensis
Simple **EEPROM programmer**. Powered by Arduino and controller over the Terminal.

<p align="center">
 <img src="./nuensis_small.gif" alt="Showcase GIF" width="900" height="300">
</p>

### What Amanuensis does...
- Lets you read individual **addresses** and whole **blocks** of memory.
- Allows you to burn a `.bin` file onto the ROM and to write **individual** values.
- Lets you erase the whole memory by **overwrtting** it with a certain value.

An all of this with through a **Command Line Interface** directly from yout laptop.

> **Note**
> Software tested on **macOS** Monterey. It might be compatible with Linux-based systems with minor/no changes.

> **Warning**
> Developped for **28c256 EEPROM** which has 32K addresses each storing 8-bit values.
> Use for other parallel EEPROMs might be possible. Check section [Use with other EEPROMs](#use-with-other-eeproms).

----


## Table of Contents
* **[How It Works](#how-it-works)**
  * **[Quick Use Guide](#quick-use-guide)**
* **[Setup](#setup)**
  * **[Hardware Setup](#hardware-setup)**
  * **[Software Installation](#installation)**
  * **[Use with other EEPROMs](#use-with-other-eeproms)**


----


## How It Works


## Quick Use Guide

To see all available options:

    nuensis read --help
    nuensis write --help


#### Writing a file (`-f`):

      nuensis write --file my_file.bin
  
`my_file.bin` should hold its contents in a **bytearray**. Example of creation of such a file:
```python
rom = bytearray([0xea]*32768)
  
rom[0x0000] = 0xaa
rom[0x002b] = 0xbb

with open("my_file.bin", "wb") as file:
  file.write(rom)
```
This results in data `0xaa` stored in address `0x0000` and `0xbb` stored in `0x002b`. 
> **Note**
> During file writting, values in the bytearray with **No-Operation** instruction (`0xea`) are ignored.
  
  
#### Writing a Single Value (`-v`):

    nuensis write --value 0123 ef  # Writes value 0xef to address 0x0123
    
    
#### Overwritting Whole Memory (`-w`):

    nuensis write --whole ea  # Writes value 0xea in all addresses.
    
> **Warning**
> This operation may take a while.


#### Reading Single Memory Address (`-a`):

    nuensis read --address 0123  # Reads address 0x0123
  
  
#### Reading Memory Range (`-r`):

    nuensis read --range 0000 002f  # Reads from address 0x0000 to 0x002f


#### Reading Whole Memory (`-w`):

      nuensis read --whole  # Reads from 0x0000 to 0x7fff
      
> **Note**
> EEPROM 28c256 has 15-bit memory registers. Its memory addresses range from 0 (`0x0000`) to 32767 (`0x7fff`).


## Setup
**Amanuensis** consists


## Hardware Setup
An Arduino Mega drives the EEPROM 28c256 through read and write cycles, as described in the [datasheet](https://eater.net/datasheets/28c256.pdf)
A custom-built shield maps the Arduino pins to the EEPROM ones as descibed in the following mapping table:

EEPROM | Pin | Arduino
-------|-----|--------

A different custom mapping can be defined in `./Amanuensis.cpp`.


## Software Installation
1. Install Arduino **Amanuensis** library by placing it in the `Arduino/libraries` folder, usually found in `~/Documents`.

<p align="center">
 <img src="./arduino_install.gif" alt="Installing Arduino library" width="700" height="174">
</p>

2. Upload `EEPROMinterface.ino` to your Arduino board.
3. Install Python requirements:
       pip install -r requirements.txt
4. Run `install.sh`.


## Recommended Writting Procedure:

1. Erase all memory contents and set them to a known value, like **No-Operation** `0xea`:

    nuensis write --whole ea
    
2. Create your own `program.bin` as described in **[Writing a Single Value](#writting-a-single-value)**

    nuensis write --file program.bin

3. Check that the contents are what you wanted them to be:



## Use With Other EEPROMs


