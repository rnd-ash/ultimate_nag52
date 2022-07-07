# PCB Design folder

In this folder you will find all the design iterations the PCB for Ultimate-NAG52 has gone through


## PCB Changelog

### V1.2 (First production release)

#### Changes

* Replaced TFT Pullup resistor (R6) from 10K to 2K
* Minor silkscreen fixes
* Added teardrops

### V1.1 (First release)

#### Changes

* Switched LM25765 Bucks to TPS5430DDAR
* Removed all electrolytic capacitors, replaced with tantalum ones
* Added smoothing outputs to INA180 current outputs (For better reading with ESP32's ADC)
* Added ESD protections
* Replaced the Type-C port with one that would not so easily break off the board
* Allow for either CP2102N or CP2104N UART bridge to be used
* Better noise filtering for ATF temp, N2 and N3 pulse counters

#### New features

* Added support for the legacy TRRS shifters found in W210 and older cars!
* Added CANBUS isolation choke
* Added additional credits to PCB silkscreen

### V1.0 (Only available to beta testers)

NOTE: This is a closed beta board, so only the schematic was made public

* Initial release board for beta testers only
