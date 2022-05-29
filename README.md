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
> During writting, **No-Operation** instructions (`0xea`) are ignored.
  
  
#### Writing a single value (`-v`):

    nuensis write --value 0123 ef  # Writes value 0xef to address 0x0123
    
    
#### Overwritting whole memory (`-w`):

    nuensis write --whole ea  # Writes value 0xea in all addresses.
    
> **Note**
> This operation may take a while.


#### Reading single memory address (`-a`):

    nuensis read --address 0123  # Reads address 0x0123
  
  
#### Reading memory range (`-r`):

    nuensis read --range 0000 002f  # Reads from address 0x0000 to 0x002f


#### Reading whole memory (`-w`):

      nuensis read --whole  # Reads from 0x0000 to 0x7fff
      
> **Note**
> EEPROM 28c256 has a 15-bit memory address. Its registers range from 0 (`0x0000`) to 32767 (`0x7fff`).


## Setup
**Amanuensis** consists


## Hardware Setup

EEPROM | Pin | Arduino
-------|-----|--------


## Software Installation

## Recommended Flow:


## Use With Other EEPROMs


