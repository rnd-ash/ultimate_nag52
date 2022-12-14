# PCB Revisions and schematics

This section outlies the various PCB versions that have been in development during this project, as well as what features the boards add. It is recommended to use the latest PCB design if you are building the TCU yourself.

* TOC line
{:toc}

## V1.3 (12/12/22)
![PCB 1.3](images/pcb13.png)
* Switched all basic SMD components (Capacitors and resistors), for higher temperature rated ones, in order to better handle engine bay installations
* Added LEDs for CAN, UART and Solenoids
* Added general purpose N-channel MOSFET on pin 6
* Added multi-use IO for pin 23 (Allows for reading G-class' output shaft sensor, or driving a speed sensor in some W124 applications)
* Added QR code on PCB silkscreen for firmware download link

### Schematic and build files

* [Schematic PDF - TBA](/404.html)
* [Gerber - TBA](/404.html)
* [BOM - TBA](/404.html)
* [Pick and place - TBA](/404.html)

## V1.2 (07/07/22)
![PCB 1.2](images/pcb12.png)
* Added legacy shifter support. This adds support for W210 generation of vehicles, and some early cars built from 2000. This includes a GPIO expander which reads data from the TRRS data lines, the Kick-down, Brake and Program button inputs, and outputs for start enable and R/P shifter solenoid.
* Switch CP2104 for CP2102 UART bridge
* Add ESD Diode to PCB
* Switched buck converters to more modern circuity
* Removed all electrolytic capacitors in favour of Tantalum ones

### Schematic and build files

```warning
Capacitors C20, C24, C31, C32, C34, and C49 must be removed! (Found to make current monitoring worse)
```

* [Schematics](https://github.com/rnd-ash/ultimate_nag52/blob/main/PCB/v1.2/schematics_1.2.pdf)
* [Gerber](https://github.com/rnd-ash/ultimate_nag52/blob/main/PCB/v1.2/gerber_1.2.zip)
* [BOM](https://github.com/rnd-ash/ultimate_nag52/blob/main/PCB/v1.2/BOM_1.2.csv)
* [Pick and place](https://github.com/rnd-ash/ultimate_nag52/blob/main/PCB/v1.2/P%26P_1.2.csv)

## V1.1 (12/12/21)
![PCB 1.1](images/pcb11.png)
* Initial PCB made publicly available for testing

### Schematic and build files

* [Schematics](https://github.com/rnd-ash/ultimate_nag52/blob/main/PCB/v1.1/schematics_1.1.pdf)
* [Gerber](https://github.com/rnd-ash/ultimate_nag52/blob/main/PCB/v1.1/gerber_1.1.zip)
* [BOM](https://github.com/rnd-ash/ultimate_nag52/blob/main/PCB/v1.1/BOM_1.1.csv)
* [Pick and place](https://github.com/rnd-ash/ultimate_nag52/blob/main/PCB/v1.1/P%26P_1.1.csv)