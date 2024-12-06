<p align="center">
 <img src="./assets/amanuensis_header.png" alt="Uploading file to EEPROM" width=90%>
</p>

### Amanuensis: Minimalistic EEPROM programmer for Ben Eater's 6502 8-bit PC.

---

### What does it do?

Amanuensis is a **Command Line Interface Tool** for easy interaction with EEPROMs like the [28c256](https://ww1.microchip.com/downloads/en/DeviceDoc/doc0006.pdf) that is used in [Ben Eater's 6502 8-bit PC course](https://www.youtube.com/watch?v=LnzuMJLZRdU).

Uses:
- Read blocks of memory.
- Write to certain addresses.
- Burn a `.bin` file to the EEPROM.
- Erase the whole memory.

<p align="center">
 <img src="./assets/nuensis_file.gif" alt="Uploading file to EEPROM" width=90%>
</p>

> **Note:**
>
> Software tested on **macOS**. It might be compatible with Linux-based systems with minor or no changes.

> **Warning:**
> Code intended for **28c256 EEPROM** which has 32K addresses each storing 8-bit values.
> Use for other parallel EEPROMs might be possible but you will have to adapt the code. Check section [Use with other EEPROMs](#use-with-other-eeproms).

---

## Table of Contents

- **[Installation](#installation)**
- **[How To Use It](#use-guide)**
- **[How It Works](#how-it-works)**
- **[DIY Shield](#diy-shield)**
- **[Pin Mapping](#diy-shield)**
- **[Recommended Writting Procedure](#recommended-writting-procedure)**
- **[Use with other EEPROMs](#use-with-other-eeproms)**

---

# Installation

1. Clone the repo:
   
       git clone https://github.com/ignigoliz/amanuensis.git

2. Create a virtual environment and install the `pyserial` library:

       pip install -r requirements.txt

3. Run the installation file to set the command-line tool.
   
       ./install.sh

> **Note:**
> Restart the Terminal window after the installation to apply the changes

4. Test the installation by opening a *new* terminal window and typing:
      
       nuensis -h

> **Warning**
> If you move the repo from the folder where it was installed, you'll need to rerun the `install.sh` script.

5. Upload `EEPROM_interface.ino` located in `src/arduino/` to your Arduino Mega board.

6. Wire the EEPROM to the Arduino as described in the [pin mapping section](#use-with-other-eeproms).

# How To Use It

To see all available options:

    nuensis read --help
    nuensis write --help

#### Writting a File (`-f`):

      nuensis write --file myfile.bin

For it to work, `myfile.bin` should be a hexdump like the following

```python
#!/usr/bin/python3
rom = bytearray([0xea]*32768)

# `0xaa` stored in address `0x0000` and `0xbb` stored in `0x002b`.
rom[0x0000] = 0xaa
rom[0x002b] = 0xbb

with open("program.bin", "wb") as file:
  file.write(rom)
```

#### Writting to a Single Address (`-a`):

    nuensis write --value 0123 ef  # Writes value 0xef to address 0x0123

#### Overwritting the Whole Memory (`-w`):

    nuensis write --whole ea  # Writes value 0xea in all addresses.

> **Warning**
> This operation may take a while:
<p align="center">
 <img src="./assets/nuensis_overwrite.gif" alt="Overwritting whole memory" width=60%>
</p>

#### Reading a Single Memory Address (`-a`):

    nuensis read --address 0123  # Reads address 0x0123

#### Reading a Memory Range (`-r`):

    nuensis read --range 0000 004f  # Reads from address 0x0000 to 0x004f

<p align="center">
 <img src="./assets/nuensis_read.gif" alt="Reading range example" width=90%>
</p>


#### Reading Whole Memory (`-w`):

      nuensis read --whole  # Reads from 0x0000 to 0x7fff


> **Note**
> EEPROM 28c256 has 15-bit memory registers. They range from 0 (`0x0000`) to 32767 (`0x7fff`).

# DIY Shield

The shield conveniently maps certain Arduino Mega pins to the correct EEPROM pins. A custom PCB design is available to recreate the shield. Alternatively, the shield can be soldered on a regular protoboard.

The hardware components needed are:

- Custom PCB.
- 28 pin ZIF socket.
- 2x 220 ohm resistors
- 1x red LED, 1x green LED.
- 32x right angle pin headers.
  
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

# Pin Mapping
The code provided expects certain Arduino Mega pins to be connected to certain EEPROM pins.

The Arduino Mega shield that I built implements such mapping. It is the following one:

> **Note**
> You can define your own mapping at `src/arduino/EEPROM_interface.ino`

| 28c256 Pin | Arduino Pin |
| :--------: | :---------: |
| WE         |   27        |
| OE         |   22        |
| CE         |   20        |
| D0         |   48        |
| D1         |   50        |
| D2         |   52        |
| D3         |   53        |
| D4         |   51        |
| D5         |   49        |
| D6         |   47        |
| D7         |   43        |
| A0         |   46        |
| A1         |   42        |
| A2         |   40        |
| A3         |   38        |
| A4         |   36        |
| A5         |   34        |
| A6         |   28        |
| A7         |   26        |
| A8         |   27        |
| A9         |   29        |
| A10        |   39        |
| A11        |   35        |
| A12        |   24        |
| A13        |   25        |
| A14        |   22        |

| LED       | Arduino Pin |
| :--------: | :---------: |
| GREEN LED  | 30          |
| RED LED    | 31          |

> **Note**
> WE: Write Enable; OE: Output Enable; CE: Chip Enable; Dx: Data x; Ax: Address x


> Green Led: Pin 30
> Red Led: Pin 31

# How It Works

Your laptop connects to Arduino, which drives the EEPROM through the appropriate electric pulses. Depending on the read or write operation, Arduino then passes the information received from the EEPROM back to your laptop.

The pulse cycles needed to drive the EEPROM can be found in the official datasheet of the [28c256 parallel EEPROM](https://eater.net/datasheets/28c256.pdf).

<p align="center">
 <img src="./assets/schema.png" alt="Communication schema" width=100%>
</p>

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
