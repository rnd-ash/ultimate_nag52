# Ultimate-NAG52 TCC Zener mod PCB for the v1.3 TCU

This is ONLY compatible with the Ultimate-NAG52 TCU V1.3!

This PCB adds an extra circuit to the TCC solenoid circuit in order to vastly improve the behaviour of the Torque converter and avoid it suddenly sticking to fully on.

In this folder, you can find all the PCB fabrication files, as well as the mount for the PCB to allow it to sit on top of the TCU without shorting anything out.

**If you are using Pin 23 on the TCU already (The IO jumpers), this modification WILL NOT WORK (Due to a conflict in pin mapping)**

## Extra info

* The holder model file is is using Inches as dimensions. Most slicers will try to use mm instead. Therefore, the scale of the board needs to be set to 25.4% of the STL file before printing.

* After installation. You must enable the TCC Zener modification in software. To do this, enter the TCU Configuration app, configure gearbox basic parameters, and set GPIO Usage to `TCC Zener mod`