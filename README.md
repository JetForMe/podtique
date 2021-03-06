
Podtique is a podcast and music player that simulates the interface and sound quality (and eventually the behavior) of an antique radio.

<img src="https://farm8.staticflickr.com/7513/16266541492_0c1e3a7c20_n.jpg" style="display: block; margin-left: auto; margin-right: auto;"/>

The code is designed to be simulated on OS X and deployed onto a [BeagleBone Black](http://beagleboard.org/black) running Linux. Currently, it builds in Xcode as a native app, and in a recent Linux as a command-line tool. Eventually it should support cross-compilation to ARM. It requires C++11.

As of this writing, the code barely plays something, and what it plays is hard-coded.

# Building the Code

Podtique uses [libao](https://www.xiph.org/ao/) and [libmpg123](http://www.mpg123.de).

## OS X

On OS X, you also need to install [Xcode](https://developer.apple.com/xcode/). Get Xcode from Apple, and use [Homebrew](http://brew.sh) for the rest:

	$ brew update
	$ brew install libao
	$ brew install mpg123
	$ brew install faad2

Open the Xcode project, and click the Run button.

## Linux

Use your favorite package manager or download libao and libmpg123. cd to the podtique directory and make:

	$ make

## Cross-Compiling for BBB

It’s not possible to cross-compile for BBB on macOS (maybe, with a *lot* of work). I’m hosting the build on Ubuntu 16.04. I need to figure out how to get ARM-targeted versions of libao and libmpg123. ALSA will be needed on the target as well. The Makefile needs to be enhanced to support cross-compiling or host compiling.

## Pin and Peripheral Assignments

Most of the I/O is done via sysfs (gross, but speed isn't critical).

* AIN0 (P9_39) is the desired frequency input.
* AIN1 (P9_40) is the desired volume input.
* AVcc (P9_32) provides power to the potentiometers.
* AGnd (P9_34) is potentiometer ground.

The digital inputs all have the internal pull-up enabled. The connected switches pull them down.

* GPIO2\_2 / 66 (P8_07 / 36) is on/off input.
* GPIO2\_3 / 67 (P8_08 / 37) is mode 1.
* GPIO2\_5 / 69 (P8_09 / 39) is mode 2.
* GPIO2\_4 / 68 (P8_10 / 38) is mode 3.
* GPIO0\_23 / 23 (P8_13 / 9) is left momentary (not currently installed).
* GPIO0\_26 / 26 (P8_14 / 10) is right momentary (not currently installed).
 
There is an output used to control the amplifier’s standby mode:

* GPIO0\_27 / 27 (P8_17 / 11) is an output to control the amplifier.

The LEDs are AdaFruit NeoPixel (24-LED ring), and driving those requires the use of the PRU.

* GPIO1\_13 (P8_11 / 13) is used by the PRU to communicate with the NeoPixel ring.

Note that the sysfs nodes for GPIO must be accessed by root. It might be possible to set up udev rules to set the group and access modes such that a non-root user can access the pins, but I couldn't figure it out. Something like KERNEL=="gpio*", GROUP="gpio", MODE="0660", but I didn't try it.

## Linux Audio Configuration

**Note:** This only applies to a USB audio dongle used in the initial prototype, and not to the custom cape.

Make sure the user running the program has audio privileges (generally means adding that user to the audio group).

The following configuration file works for the Audio-Technica USB audio dongle I used.

	$ cat /etc/asound.conf 
	pcm.!default {
	  type plug
	  slave {
	    pcm "hw:1,0"
	  }
	}
	ctl.!default {
	  type hw
	  card 1
	}
	
# BBB Cape

There is a custom Cape for use with the BeagleBone Black. The first version, Podtique1.osm, was developed using [Osmond](http://osmondpcb.com) for OS X, which is available for free.

## Podtique Cape 1

This cape has the minimal hardware needed to enable Podtique functionality:

* Audio CODEC
* Audio amplifier
* Filtered power for audio circuitry
* NeoPixel level shifting
* Front panel interface

# Radio Notes

* Screws in bottom to hold everything in place.
* Add battery switch and wiring harness.
* Add battery.
* Make sure I can talk to it without a network present (straight cable connection).
* Try turning off eth0 config at boot and config after (wicd/connman).
* modprobe uio_pruss at boot.

## Done

* Turn off LEDs on everything.
* Add dial LEDs as needed.
* Add amp control GPIO to avoid power-on pop.
* Some light can be seen through speaker grill. Add opaque cover behind.
* Make back plate rectangular and nearly full height. Mount BBB and amp to it. * Holes for zip tie for USB audio adapter?
* Add mounting holes for 5 VDC regulator (eventually part of BBB cape).
* Cut opening to allow long potentiometers to be fit into front plate.
* Move tuning potentiometer to middle wall.
* Need place for batteries!
* Move front plate back a few mm so slots become holes.
* Cut pulleys out of wood. Make middle wall larger to allow for stops? Means needs good coupling with tuning axle. Instead, use all 10 turns of tuning axle, let potentiometer do the stopping, and make the ratio be 20:1 (half turn of dial for 10 turns of pot).
* Make sure top of back isn't so tall as to prevent assembly.


## Later Version

* Make tube amplifier to show tubes in back?
* Investigate sleep modes for BBB.
* Mid-pass filter effect?
* Fast boot (U-Boot Falcon mode?)
