---
title: home
layout: default
---

# lagtest - How slow is your display?

<table><tr><td>
<img src="/assets/img/warning.png">
</td><td>
Warning: lagtest.org is not yet complete, but we're developing in
public. To see our work in progress, check <a href="https://github.com/strawlab/lagtest">our github
repository</a>.
</td></tr></table>

## New displays are slow

Old-style CRT displays may be big and bulky, but there is practically
zero delay between video card output and luminance change on the
screen -- the video output analog voltages when directly controlled
the intensity of the electron beam. With the rise of new display
technologies such as LCDs and DLPs, this has changed for several
reasons (resampling the image and other onboard image processing,
inherent slowness due to the physics of the device, and so on).

## How slow is your display?

The [lagtestino](lagtestino) is a USB-connected photodiode. Connect it to your
computer, run the lag-test program, follow the on screen instructions,
and you have your latency estimate.
