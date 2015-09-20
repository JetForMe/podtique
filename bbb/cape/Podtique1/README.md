# BBB Expansion Header Usage

Pin and Peripheral Assignments
------------------------------

Most of the I/O is done via sysfs (gross, and we need to look into speeding up reading the ports so volume and tuning changes are smoother).

* AIN0 (P9_39) is the desired frequency input (PANEL 3)
* AIN1 (P9_40) is the desired volume input (PANEL 4).
* AIN2 (P9_37) is the desired tone (or other) input (PANEL 5) (currently unsupported in the software).
* GPIO3\_19 (P9_27) enables the 1.8V regulator to power the potentiometers.
* AGnd (P9_34) is potentiometer ground.

The digital inputs all have the internal pull-up enabled. The connected switches pull them down.

* GPIO2\_11 / xx (P8_42 / xx) is on/off input (PANEL 6).
* GPIO2\_6 / xx (P8_45 / xx) is mode 1 (PANEL 7).
* GPIO2\_8 / xx (P8_43 / xx) is mode 2 (PANEL 8).
* GPIO2\_7 / xx (P8_46 / xx) is left momentary (PANEL 9) (not currently installed).
* GPIO2\_9 / xx (P8_44 / xx) is right momentary (PANEL 10) (not currently installed).
 
There is an output used to control the amplifier’s standby mode:

* GPIO0\_27 / 27 (P8_17 / 11) is an output to control the amplifier.

The LEDs are AdaFruit NeoPixel (24-LED ring), and driving those requires the use of the PRU.

* GPIO1\_13 (P8_11 / 13) is used by the PRU to communicate with the NeoPixel ring.

Note that the sysfs nodes for GPIO must be accessed by root. It might be possible to set up udev rules to set the group and access modes such that a non-root user can access the pins, but I couldn't figure it out. Something like KERNEL=="gpio*", GROUP="gpio", MODE="0660", but I didn't try it.

Audio CODEC
-----------
i2c address is 0x30. Shares i2c bus 2 with EEPROM, which defaults to 0x57.

New Header Pins
---------------
### P8

|Header|GPIO|$PINS	|GPIO #|Mode|Use|
|---|-------|----|-----|------|----|
|P8-11|gpio1[13]|13|45|x|NeoPixel data out|
|P8-42			|GPIO2_11	|9		|23			|
|P8-45			|GPIO2_6	|

* GPIO2\_11 / xx (P8_42 / xx) is on/off input (PANEL 6).
* GPIO2\_6 / xx (P8_45 / xx) is mode 1 (PANEL 7).
* GPIO2\_8 / xx (P8_43 / xx) is mode 2 (PANEL 8).
* GPIO2\_7 / xx (P8_46 / xx) is left momentary (PANEL 9) (not currently installed).
* GPIO2\_9 / xx (P8_44 / xx) is right momentary (PANEL 10) (not currently installed).
 
Amplifier:

* GPIOxx / xx (P8_12 / xx) input, amplifier fault (nFAULT, active low).
* GPIOxx / xx (P8_13 / xx) output, amplifier shutdown (nSD, active low).

CODEC:

* GPIOxx / xx (P9_16 / xx) output, CODEC reset (nRESET, active low).
* GPIOxx / xx (P9_20 / xx) CODEC SDA.
* GPIOxx / xx (P9_19 / xx) cCODEC SCL.

The LEDs are AdaFruit NeoPixel (24-LED ring), and driving those requires the use of the PRU.

* GPIO1\_13 (P8_11 / 13) is used by the PRU to communicate with the NeoPixel ring.


### P9

|Header|GPIO|$PINS	|GPIO #|Mode|Use|
|---|-------|----|-----|------|----|
|P9-27|gpio3[19]|105|115|7|Front panel 1.8V power|
|P9-14|gpio1[18]|18|50|7|CODEC core 1.8V and amp 3.3V|

Cape Connectors
---------------

### Vin

Supply 12 - 14 V from a regulated supply. This rail goes directly to the audio power amplifier, so it should be relatively clean.

Pin 1: GND
Pin 2: Power In

### NEOPXL

Three-pin header goes to NeoPixel LEDs to illuminate the radio.

* Pin 1: 5V power (at least an amp available).
* Pin 2: GND
* Pin 3: Data (level shifted from GPIO1\_13, P8_11/13).

### PANEL

2x5 10-pin header provides power and ground to a wiring harness that can support up to three potentiometers, and five switches. The potentiometers should be at least 5K, one end to 1.8V power, one to ground, and the wiper to the appropriate input. The switches should short to ground, and are pulled up on the board.

* Pin 1: Agnd
* Pin 2: 1.8 V (regulator enabled from GPIO3\_19, P9_27)
* Pin 3: Frequency input
* Pin 4: Volume input
* Pin 5: Other input (tone?)
* Pin 6: On/off
* Pin 7: Mode 1
* Pin 8: Mode 2
* Pin 9: Left Momentary
* Pin 10: Right Momentary

