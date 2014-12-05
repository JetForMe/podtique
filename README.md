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
