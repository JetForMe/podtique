Podtique
========

A podcast and music player that simulates the interface and sound quality of an antique radio.

The code is designed to be simulated on OS X and deployed onto a [BeagleBone Black](http://beagleboard.org/black) running Linux. Currently, it builds in Xcode as a native app, and in a recent Linux as a command-line tool. Eventually it should support cross-compilation to ARM.

Prerequisites
=============

On OS X, you need to install [Xcode](https://developer.apple.com/xcode/), [libao](https://www.xiph.org/ao/), and [libmpg123](http://www.mpg123.de). Get Xcode from Apple, and se [homebrew](http://brew.sh) for the rest:

	$ brew update
	$ brew install libao
	$ brew install mpg123
