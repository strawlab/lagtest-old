lagtest - How slow is your display?
===================================

New displays are slow
=====================

Old-style CRT displays may be big and bulky, but there is practically
zero delay between video card output and luminance change on the
screen -- the video output analog voltages when directly controlled
the intensity of the electron beam. With the rise of new display
technologies such as LCDs and DLPs, this has changed for several
reasons (resampling the image and other onboard image processing,
inherent slowness due to the physics of the device, and so on).

How slow is your display?
=========================

The lagtestino is a USB-connected photodiode. Connect it to your
computer, run the lag-test program, follow the on screen instructions,
and you have your latency estimate.

Currently, you need to build the lagtestino yourself. (We would like
to offer pre-made devices in the future.)

Building and programming the lagtestino
=======================================

The parts cost less than $40, and it's a very simple project to make. 

 * Arduino USB board (about $30, model Uno R3 SMD [@mouser](http://www2.mouser.com/ProductDetail/Arduino/A000073/?qs=8PMfw1Pw72XWXcmEu0o8%2fg1FF4%252btE0PZ6yLj4ZrSrAoo7OhNftgfdw%3d%3d))
 * photodiode and amplifier (about $5, model TAOS TSL252R-LF [@mouser](http://www2.mouser.com/ProductDetail/TAOS/TSL252R-LF/?qs=Ppq%252bQS%252b9qgLR5SNs1yN%252b2XDN8BMyJvUu))

Plug in the GND, +5V, and Vout of the photodetector into the GND, +5V,
and AIN0 of the Arduino.

[![CircuitLab Schematic vhds6p](https://www.circuitlab.com/circuit/vhds6p/screenshot/540x405/)](https://www.circuitlab.com/circuit/vhds6p/lagtestino/).

To program the lagtestino, get the firmware source code [as a .zip
archive](https://github.com/strawlab/lagtest/zipball/master) or via
[our github repository](http://github.com/strawlab/lagtest). [Here's a
guide that describes what you need to
do.](http://arduino.cc/it/Guide/Windows) Now, after getting the blink
example to work, open the lagtestino project and upload it to your
device.

Theory of operation
===================

The most significant technical challenge is synchronizing the clock of
your computer with the luminance samples made by the USB device.

Sub-millisecond synchronization between your computer and USB device
--------------------------------------------------------------------

To allow your computer to determine exactly when samples were
acquired, a simple protocol involving timestamp exchange is
implemented between the host computer and the USB device. The host
sends a request to the arduino, keeping note of the time the request
was made (`T_host_request`). The lagtestino firmware responds to this
request by sending the current value of its clock (`C_arduino`). The
host records the time of arrival of this new data
(`T_host_receive`). By assuming that delays are symmetric and only
accepting responses that occur within a short latency (thus bounding
maximal error), the host calculates that the device's timetamp
occurred at `T_arduino_estimate`:

    request_response_roundtrip_duration = T_host_receive - T_host_request
    if request_response_roundtrip_duration < maximal_acceptable_error:
        T_arduino_estimate = (T_host_request + T_host_receive) / 2

So `T_arduino_estimate` is the best guess as to when `C_arduino` was
sampled, in the host computer's timebase. After many such samples, we
have two corresponding vectors:

    T = [ T_arduino_estimate[0], T_arduino_estimate[1], ... ]
    C = [ C_arduino[0],          C_arduino[1],          ... ]

We can now model the relation between the two clocks with:

    T = gain*C + offset

The values of `gain` and `offset` can be found by a least-squares fit
from the acquired vectors `T` and `C`. Doing so allows conversion from
any arduino clock value (`C_arduino`) to be directly converted to a
timestamp of the host computer.

This is concept is a simplification of the ideas contained in the IEEE
1588 specification but is sufficient for acheiving sub-millisecond
timing precision between an arduino and unpriviledged software.
