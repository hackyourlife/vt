DEC VTs for AVR
===============

This firmware can be uploaded to an ODROID SHOW2 module to create a simple VT52
or VT220.

Features
========

* Complete VT52 support (no keyboard and no printer of course)
* Rudimentary VT220 support
* Reset button
* Buttons to switch between VT52 and VT220 mode
* 80x40 characters
* 9600 Baud
* 8MHz SPI transfer to the display controller

Missing Features
================

* ReGIS
* Sixel-Graphics
* Tek4014

Impossible Features
===================

Those features cannot be implemented because of hardware restrictions.

* Screen buffer (does not fit into 2K RAM)
* Scrolling (use screen: it can emulate scrolling)
* Keyboard
* Printer

Bugs
====

* There is an interference between the UART receiver and the SPI transmitter.
  This causes graphical glitches because (control) characters are lost
  sometimes. The bug can be prevented if you wait after erase operations until
  they have finished.

Notes
=====

If you use the VT52 mode you should definitively start GNU screen to make it
useful.

Photos
======

![pacman update on VT52](https://raw.githubusercontent.com/hackyourlife/vt/master/doc/pacman-vt52.jpg)
