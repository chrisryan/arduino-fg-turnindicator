Arduino Turn Indicator
======================

This project is to connect an Arduino to the Flight Gear flight simulator
to create a turn indicator. The project uses and Arduino Uno with a servo,
led, resitor, and two potentiometers. The Servo controlls the airplane angle
and the led shows the failure flag based on the information read from the
flight simulator. The two potentiometers are used to adjust the level and
standard rate turn positions.

Circuit
-------

    V5 ---- [ POT ] ---- GND
               |
            Analog 0

    V5 ---- [ POT ] ---- GND
               |
            Analog 1

    V5 ---- [ R 330 ohm ] ---- [ LED ] ---- GND

    V5 ---- Servo PWR
    GND --- Servo GND
    Digital 9 ---- Servo Signal

Flight Gear
-----------

To have flight gear communicate with the arduino you must first copy the file
arduinofgturnindicator.xml to the Protocol directory of flight gear. After
that you must start flight gear with the following command line, adjusting the
COM port name appropriately for your system.

  --generic=serial,out,30,COM9,9600,arduinofgturnindicator

Note that on Windows that COM ports higher than 9 must be named as \\.\COM10 as
an example. On linux or Mac you will have to use the appropriate naming scheme
for your operating system.

Functionality
-------------

When the circuit starts up it will go into a test mode where the failure led
will illuminate and then the servo will move to several predefined positions.
After that it will return to level position and remain in failure mode until
Flight Gear starts to transmit data to the circuit.

The first pot can be used to adjust the wings level or no-turn position while
the second pot can be used to adjust the standard rate turn position.

