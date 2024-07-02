<p align="center">
 <img src="./assets/amanuensis_header.png" alt="Uploading file to EEPROM" width=90%>
</p>

### AMANUENSIS: Minimalistic EEPROM programmer for Ben Eater's 6502 8-bit PC.

---

### What does it do?

Amanuensis is a **command-line interface** to control EEPROMs like the 28c256 used in [Ben Eater's 6502 8-bit PC course](https://www.youtube.com/watch?v=LnzuMJLZRdU). It can:
- Read individual addresses.
- Read blocks of memory.
- Write to specific addresses.
- Write a `.bin` file into the EEPROM.
- Erase the memory.

<p align="center">
 <img src="./assets/nuensis_file.gif" alt="Uploading file to EEPROM" width=90%>
</p>

> **Note:**
> Software tested on **macOS**. It might be compatible with Linux-based systems with minor or no changes.

> **Warning:**
> Developped for **28c256 EEPROM** which has 32K addresses each storing 8-bit values.
> Use for other parallel EEPROMs might be possible. Check section [Use with other EEPROMs](#use-with-other-eeproms).

---

## Table of Contents

- **[Installation](#installation)**
- **[Use Guide](#use-guide)**
- **[How It Works](#how-it-works)**
- **[DIY Shield](#diy-shield)**
- **[Recommended Writting Procedure](#recommended-writting-procedure)**
- **[Use with other EEPROMs](#use-with-other-eeproms)**

---

# Installation

1. Clone this repo:
   
       git clone https://github.com/ignigoliz/amanuensis.git

2. Install Python requirements:

       pip install -r requirements.txt

3. In a terminal, go to the cloned repo and execute the install file:
   
       ./install.sh

> **Note:**
> Restart the Terminal window after the installation to apply the changes

4. Test the installation by opening a **new** terminal window and typing:
      
       nuensis -h

> **Warning**
> Moving the repo folder will break the paths of the binaries. To avoid this, follow the steps in **[Moving the `amanuensis` folder](#moving-the-amanuensis-folder)**.

5. Add the Amanuensis Library to Arduino by copying the `AmanuensisLib` folder located in `src/Arduino/` to `~/Documents/Arduino`:

<p align="center">
 <img src="./assets/arduino_install.gif" alt="Installing AmanuensisLib (Arduino library)" width=80%>
</p>

6. Upload `EEPROM_interface.ino` located in `src/EEPROM_interface/` to your Arduino board.

# Use Guide

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

# How It Works

Your laptop connects to Arduino, which drives the EEPROM through the appropriate electric pulses. Depending on the read or write operation, Arduino then passes the information received from the EEPROM back to your laptop.

The pulse cycles needed to drive the EEPROM can be found in the official datasheet of the [28c256 parallel EEPROM](https://eater.net/datasheets/28c256.pdf).

<p align="center">
 <img src="./assets/schema.png" alt="Communication schema" width=100%>
</p>


# DIY Shield

The shield conveniently maps certain Arduino Mega pins to the correct EEPROM pins. A custom PCB design is available to recreate the shield. Alternatively, the shield can be soldered on a regular protoboard.

The hardware components needed are:

- Custom PCB.
- 28 pin ZIF socket.
- 2x 220 ohm resistors
- 1x red LED, 1x green LED.
  
<p align="center">
 <img src="./assets/new_shield_components.jpg" alt="Hardware components" width=50%>
</p>

The PCB maps Arduino Mega pins to the ZIF socket where the EEPROM is connected. The Gerber files can be found at `pcb/`.

<p align="center">
 <img src="./assets/new_pcb.jpg" alt="Top and bottom view of the PCB of the shield" width=50%>
</p>

<p align="center">
 <img src="./assets/gerber.png" alt="Screenshor of the gerber file" width=50%>
</p>

The shield can be crafted on a protoboard as well:

<p align="center">
 <img src="./assets/overall_hardware.png" alt="Hardware components" width=75%>
</p>

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

#### Moving the `amanuensis` folder

The Command Line Interface (CLI) relies on finding the PATH of the `amanuensis` folder, which was set when running `install.sh`. Therefore, if the folder is moved around, the CLI commands will fail. To solve it:

1. Move the `amanuensis` folder to the new desired destination.
2. Open `./bash_profile` and delete the entry `source [your-path]/nuensis.sh`
3. cd to the new folder location and re-run `./install.sh`.

# Recommended Writting Procedure:

1. Erase all memory contents and set them to a known value, like **No-Operation** `0xea`:

       nuensis write --whole ea

2. Create your own `program.bin` as described in **[Writting a File](<#writting-a-file-(f)>)**

       nuensis write --file program.bin

3. Check that the contents are what you wanted them to be:

# Use With Other EEPROMs

`Amanuensis` should work with other **parallel** EEPROMs as long as they respond to the same read/write pulse cycles as the **28c256** (for example the 28c64):

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
