# Amanuensis
Simple EEPROM programmer, powered by Arduino and controller over the Terminal.

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
- Writing a file:
  `nuensis write -f my_file.bin`
  
  `nuensis` is able to write files whose contens are **bytearrays**.
  > **Note**
  > **No-Operation** instructions (`0xea`) are ignored.

  Example of use:
  ```python
  rom = bytearray([0xea]*32768)
  
  rom[0x0000] = 0xaa
  rom[0x002b] = 0xbb

  with open("my_file.bin", "wb") as file:
    file.write(rom)
  ```
  This results in data `0xaa` stored in address `0x0000` and `0xbb` stored in `0x002b`. The remaining `0xea` data stored in rom bytearray are ignored.
  

## Setup


## Hardware Setup


## Software Installation


## Use With Other EEPROMs


