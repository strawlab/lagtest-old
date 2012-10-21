---
title: lagtestino
layout: default
---

Building and programming the lagtestino
=======================================

The lagtestino ("lagtest" + "Arduino") is the name of the hardware
required to accurately test lag. You build it yourself. The parts cost
less than $40, and it's very simple to make.

The parts are:

 * Arduino USB board (about $30, model Uno R3 SMD [@mouser](http://www2.mouser.com/ProductDetail/Arduino/A000073/?qs=8PMfw1Pw72XWXcmEu0o8%2fg1FF4%252btE0PZ6yLj4ZrSrAoo7OhNftgfdw%3d%3d))
 * photodiode and amplifier (about $5, model TAOS TSL252R-LF [@mouser](http://www2.mouser.com/ProductDetail/TAOS/TSL252R-LF/?qs=Ppq%252bQS%252b9qgLR5SNs1yN%252b2XDN8BMyJvUu))

<img src="/assets/img/lagtestino-fritzing.png" >
<img src="/assets/img/taos-cable.jpg" width="400" height="248">


Plug in the GND, +5V, and Vout of the photodetector into the GND, +5V,
and AIN0 of the Arduino. We have a
[schematic](https://www.circuitlab.com/circuit/vhds6p/lagtestino/) at
CircuitLab.

To program the lagtestino, get the firmware source code [as a .zip
archive](https://github.com/strawlab/lagtest/zipball/master) or via
[our github repository](http://github.com/strawlab/lagtest). [Here's a
guide that describes what you need to
do.](http://arduino.cc/it/Guide/Windows) Now, after getting the blink
example to work, open the lagtestino project and upload it to your
device.
