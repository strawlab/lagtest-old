---
title: home
layout: default
---

# lagtest - How slow is your display?

<table><tr><td>
<img src="/assets/img/warning.png" />
</td><td>
Warning: lagtest.org is not yet complete, but we're developing in
public. To see our work in progress, check <a href="https://github.com/strawlab/lagtest">our github
repository</a>.
</td></tr></table>

## New displays are slow

Old-style CRT displays may be big and bulky, but there is practically
zero delay between video card output and luminance change on the
screen -- the analog voltages output by the video card directly
control the intensity of the electron beam. With the rise of new
display technologies such as LCDs and DLPs, this has changed for
several reasons (resampling the image and other onboard image
processing, inherent slowness due to the physics of the device, and so
on). The purpose of [lagtest.org](http://lagtest.org) is to let you
measure the latency from a computer program sending a draw command to
the result appearing on your screen.

<table>
  <tr><td align="center">
    <img src="/assets/img/lag-drawing.png" />
  </td></tr>
  <tr><td>
    <strong>A simple USB device (based on an <a href="http://www.arduino.cc/">Arduino</a> and a
    photodiode) lets you measure latency from software to photons.</strong>
  </td></tr>
</table>

## How slow is your display?

The [lagtestino](lagtestino) is a USB-connected photodiode. Connect it to your
computer, run the lag-test program, follow the on screen instructions,
and you have your latency estimate.
