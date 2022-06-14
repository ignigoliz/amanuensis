<p align="center">
 <img src="./assets/amanuensis_header.png" alt="Uploading file to EEPROM" width=90%>
</p>

---

Simple **EEPROM programmer**. Powered by Arduino and controller over the Terminal. Useful for those following [Ben Eater's 6502 pc course](https://www.youtube.com/watch?v=LnzuMJLZRdU).

### What Amanuensis does...

- Lets you read individual **addresses** and whole **blocks** of memory.
- Allows you to burn a `.bin` file onto the EEPROM and to write **individual** values to specific addresses.
- Lets you erase the whole memory by **overwrting** it with a certain value.

<p align="center">
 <img src="./assets/nuensis_file.gif" alt="Uploading file to EEPROM" width=90%>
</p>

All of this with through a **Command Line Interface** directly from yout laptop.

> **Note**
> Software tested on **macOS** Monterey. It might be compatible with Linux-based systems with minor/no changes.

> **Warning**
> Developped for **28c256 EEPROM** which has 32K addresses each storing 8-bit values.
> Use for other parallel EEPROMs might be possible. Check section [Usage with other EEPROMs](#usage-with-other-eeproms).

---

## Table of Contents

- **[How It Works](#how-it-works)**
  - **[Quick Use Guide](#quick-use-guide)**
- **[Setup](#setup)**
  - **[Hardware Setup](#hardware-setup)**
  - **[Software Installation](#software-installation)**
  - **[Recommended Writting Procedure](#recommended-writting-procedure)**
  - **[Use with other EEPROMs](#use-with-other-eeproms)**

---

# How It Works

Your laptop's Command Line Interface interacts with Arduino over the serial port. At the same time, the Arduino drives **read/write pulse cycles** onto the parallel EEPROM. It sends back the information to Arduino, which passes it back to your laptop's Command Line Interface.

These read/write pulse cycles can be found the [datasheet](https://eater.net/datasheets/28c256.pdf) of the **28c256 parallel EEPROM**.

<p align="center">
 <img src="./assets/schema.png" alt="Communication schema" width=100%>
</p>

## Quick Use Guide

To see all available options:

    nuensis read --help
    nuensis write --help

#### Writting a File (`-f`):

      nuensis write --file program.bin

`program.bin` should hold its contents in a **bytearray**. Example of creation of such a file:

```python
#!/usr/bin/python3
rom = bytearray([0xea]*32768)

rom[0x0000] = 0xaa
rom[0x002b] = 0xbb

with open("program.bin", "wb") as file:
  file.write(rom)
```

This results in data `0xaa` stored in address `0x0000` and `0xbb` stored in `0x002b`.

> **Note**
> Before writing an address, its content is retrieved. If it coincides with the desired value, the write operation is skiped. This is to avoid unnecessary write cycles, which are relatively slow (~10ms).

#### Writting a Single Value (`-v`):

    nuensis write --value 0123 ef  # Writes value 0xef to address 0x0123

#### Overwritting Whole Memory (`-w`):

    nuensis write --whole ea  # Writes value 0xea in all addresses.

> **Warning**
> This operation may take a while:
<p align="center">
 <img src="./assets/nuensis_overwrite.gif" alt="Overwritting whole memory" width=60%>
</p>

#### Reading Single Memory Address (`-a`):

    nuensis read --address 0123  # Reads address 0x0123

#### Reading Memory Range (`-r`):

    nuensis read --range 0000 004f  # Reads from address 0x0000 to 0x004f

<p align="center">
 <img src="./assets/nuensis_read.gif" alt="Reading range example" width=90%>
</p>


#### Reading Whole Memory (`-w`):

      nuensis read --whole  # Reads from 0x0000 to 0x7fff


> **Note**
> EEPROM 28c256 has 15-bit memory registers. They range from 0 (`0x0000`) to 32767 (`0x7fff`).


# Setup

## Hardware Setup

<p align="center">
 <img src="./assets/overall_hardware.png" alt="Hardware components" width=75%>
</p>

An **Arduino Mega** drives the **EEPROM 28c256** through a custom made **Shield**:

<p align="center">
 <img src="./assets/shield.png" alt="Shield description" width=50%>
</p>

The **Shield** performs the following pin mapping:

|    EEPROM     | Pin | Arduino |
| :-----------: | :-: | :-----: |
| Write Enable  | WE  |   32    |
| Output Enable | OE  |   52    |
|  Chip Enable  | CE  |   29    |
|    Data 0     | D0  |   49    |
|    Data 1     | D1  |   41    |
|    Data 2     | D2  |   39    |
|    Data 3     | D3  |   35    |
|    Data 4     | D4  |   33    |
|    Data 5     | D5  |   23    |
|    Data 6     | D6  |   25    |
|    Data 7     | D7  |   27    |
|   Address 0   | A0  |   47    |
|   Address 1   | A1  |   45    |
|   Address 2   | A2  |   51    |
|   Address 3   | A3  |   50    |
|   Address 4   | A4  |   44    |
|   Address 5   | A5  |   46    |
|   Address 6   | A6  |   48    |
|   Address 7   | A7  |   40    |
|   Address 8   | A8  |   24    |
|   Address 9   | A9  |   26    |
|  Address 10   | A10 |   53    |
|  Address 11   | A11 |   28    |
|  Address 12   | A12 |   38    |
|  Address 13   | A13 |   32    |
|  Address 14   | A14 |   36    |

A different mapping might be defined in `./src/Arduino/Amanuensis/Amanuensis.cpp`.

## Software Installation

1. Clone or download this repo and place it in your **path** of preference (e.g. `./Documents/`).

> **Warning**
> Moving the repo folder will break the paths of the binaries. To avoid this, follow the steps in **[Moving the `amanuensis` folder](#moving-the-amanuensis-folder)**.

#### Arduino setup

2. Install Arduino **Amanuensis library** by placing the `./src/Arduino/AmanuensisLib/` folder in your system's `Arduino/libraries/` folder, usually found in `~/Documents/`.

<p align="center">
 <img src="./assets/arduino_install.gif" alt="Installing AmanuensisLib (Arduino library)" width=80%>
</p>

3. Upload `EEPROM_interface.ino` to your Arduino board.

#### Laptop setup

4. Install Python requirements:

       pip install -r requirements.txt

5. Move to the amanuensis folder, wherever you placed it, (`cd [your-path]/amanuensis`) and run `./install.sh`.

To test the installation, open a **new** terminal window and type `nuensis -h`.

#### Moving the `amanuensis` folder

The Command Line Interface (CLI) relies on finding the PATH of the `amanuensis` folder, which was set when running `install.sh`. Therefore, if the folder is moved around, the CLI commands will fail. To solve it:

1. Move the `amanuensis` folder to the new desired destination.
2. Open `./bash_profile` and delete the entry `source [your-path]/nuensis.sh`
3. cd to the new folder location and re-run `./install.sh`.

## Recommended Writting Procedure:

1. Erase all memory contents and set them to a known value, like **No-Operation** `0xea`:

       nuensis write --whole ea

2. Create your own `program.bin` as described in **[Writting a File](<#writting-a-file-(f)>)**

       nuensis write --file program.bin

3. Check that the contents are what you wanted them to be:

## Use With Other EEPROMs

`Amanuensis` should work with other **parallel** EEPROMs as long as they respond to the same read/write pulse cycles as the **28c256** (e.g. 28c64):

<p align="center">
 <img src="./assets/nuensis_pulses.png" alt="Pulses for reading/writting operations" width=100%>
</p>

However, the code might have to be changed to fit your particular EEPROM needs. This is a quick checklist:

- [ ] Confirm that the working voltage of your EEPROM is correctly set (in this case, Vcc of 5V is provided by Arduino).
- [ ] Revisit the **pin mapping**, defined in `amanuensis/src/Amanuensis.cpp`.
- [ ] Set `MAX_ADDRESS` of your EEPROM in `./src/ArduinoInterface.py` and in `./src/EEPROM_interface.ino` (e.g., for 15-bit addresses, `MAX_ADDRESS=32767`).

<p align="right">
  <img src="https://komarev.com/ghpvc/?username=amanuensis&color=orange&style=flat-square&label=VISITOR+COUNT" alt=”tomkaX” />
</p>
