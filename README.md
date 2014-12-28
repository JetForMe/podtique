Podtique
========

A podcast and music player that simulates the interface and sound quality of an antique radio.

The code is designed to be simulated on OS X and deployed onto a [BeagleBone Black](http://beagleboard.org/black) running Linux. Currently, it builds in Xcode as a native app, and in a recent Linux as a command-line tool. Eventually it should support cross-compilation to ARM.

As of this writing, the code barely plays something, and what it plays is hard-coded.

Building the Code
=================

Podtique uses [libao](https://www.xiph.org/ao/) and [libmpg123](http://www.mpg123.de).

OS X
----

On OS X, you also need to install [Xcode](https://developer.apple.com/xcode/). Get Xcode from Apple, and use [Homebrew](http://brew.sh) for the rest:

	$ brew update
	$ brew install libao
	$ brew install mpg123

Open the Xcode project, and click the Run button.

Linux
-----

Use your favorite package manager or download libao and libmpg123. cd to the podtique directory and make:

	$ make

Cross-Compiling for BBB
-----------------------

It’s not yet possible to cross-compile for BBB. I’m hosting the build on Ubuntu 14.04. I need to figure out how to get ARM-targeted versions of libao and libmpg123. ALSA will be needed on the target as well. The Makefile needs to be enhanced to support cross-compiling or host compiling.

Pin and Peripheral Assignments
------------------------------

Most of the I/O is done via sysfs (gross, but speed isn't critical).

* AIN0 (P9_39) is the desired frequency input.
* AIN1 (P9_40) is the desired volume input.
* AVcc (P9_32) provides power to the potentiometers.
* AGnd (P9_34) is potentiometer ground.

The digital inputs all have the internal pull-up enabled. The connected switches pull them down.

* GPIO2\_2 (P8_07 / 36) is on/off input.
* GPIO2\_3 (P8_08 / 37) is mode 1.
* GPIO2\_5 (P8_09 / 39) is mode 2.
* GPIO2\_4 (P8_10 / 38) is mode 3.

There is an output used to control the amplifier’s standby mode:

* GPIO0\_23 (P8_13 / 9) is an output to control the amplifier.

The LEDs are AdaFruit NeoPixel (24-LED ring), and driving those requires the use of the PRU.

* GPIO1\_13 (P8_11 / 13) is used by the PRU to communicate with the NeoPixel ring.

 
Radio Notes
===========

Turn off LEDs on everything.
Add dial LEDs as needed.
Add amp control GPIO to avoid power-on pop.
Some light can be seen through speaker grill. Add opaque cover behind.
Mounting holes in bottom plate? Maybe just wood screws up from bottom into it.

Mid-pass filter effect?

Done
----
Make back plate rectangular and nearly full height. Mount BBB and amp to it. Holes for zip tie for USB audio adapter?
Add mounting holes for 5 VDC regulator (eventually part of BBB cape).
Cut opening to allow long potentiometers to be fit into front plate.
Move tuning potentiometer to middle wall.
Need place for batteries!
Move front plate back a few mm so slots become holes.
Cut pulleys out of wood. Make middle wall larger to allow for stops? Means needs good coupling with tuning axle. Instead, use all 10 turns of tuning axle, let potentiometer do the stopping, and make the ratio be 20:1 (half turn of dial for 10 turns of pot).
Make sure top of back isn't so tall as to prevent assembly.


Later Version
=============
Make tube amplifier to show tubes in back?
Investigate sleep modes for BBB.
