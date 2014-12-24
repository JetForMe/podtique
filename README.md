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


Radio Notes
===========

Tuning is a bit sensitive. Widen band? 10-turn pot will solve. GET 10-turn POT!
Weird moment of silence when tuning in Beyond Belief.

Turn off LEDs on everything.
Add dial LEDs as needed.
Add amp control GPIO to avoid power-on pop.
Investigate sleep modes for BBB.
Some light can be seen through speaker grill. Add opaque cover behind.
Mounting holes in bottom plate? Maybe just wood screws up from bottom into it.
Make sure top of back isn't so tall as to prevent assembly.

Work on mid-pass filter effect?

Done
----
Make back plate rectangular and nearly full height. Mount BBB and amp to it. Holes for zip tie for USB audio adapter?
Add mounting holes for 5 VDC regulator (eventually part of BBB cape).
Cut opening to allow long potentiometers to be fit into front plate.
Move tuning potentiometer to middle wall.
Need place for batteries!
Move front plate back a few mm so slots become holes.
Cut pulleys out of wood. Make middle wall larger to allow for stops? Means needs good coupling with tuning axle. Instead, use all 10 turns of tuning axle, let potentiometer do the stopping, and make the ratio be 20:1 (half turn of dial for 10 turns of pot).



Later Version
=============
Make tube amplifier to show tubes in back?
