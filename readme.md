# Ravine Despoiler

a Arduboy game developed by Ben Combee

<img src="screenshot.png" alt="screenshot with saucer beaming up a cow">

This game was originally started for the black & white Pebble watch.  I ended up
getting some basic animation going and had it as a watch face, but I never implemented
the game controls.  After doing my first Arduboy game jam (which required all new
programming), I figured I'd come back to this one, porting over some of my Pebble code
and assets to the new platform.

It's based on the Atari arcade game Canyon Bomber.  You can speed up or slow down
your plane and drop bombs in the canyon to knock out boulders for points.  Missing
rocks will result in losing time, and when you run out of time, the game's over.  If
you clear a canyon, it magically refills.

## THINGS TO IMPLEMENT

* end-of-level fanfare
* explainer screen
* animated ravine filling at start of level
* plane flying sound effect
* bomb dropping sound effect
* explosion sound effect
* level timer ???
* better score display

## LICENSE

Copyright (C) 2020, Ben Combee
Released under Apache 2.0 license

Physics code originally adapted from https://github.com/Pharap/PhysixArduboy/blob/master/LICENSE
used under Apache 2.0 license
Copyright (C) 2018 Pharap (@Pharap)

The code incorporates an adaptation of the DrawLine routine from the Arduboy2 library,
used under the BSD 3-part license

The code also uses an abbreviated form of the Font4x6.h header 
Copyright (c) 2019, Press Play On Tape
All rights reserved.

Arduboy2 library:
Copyright (c) 2016-2020, Scott Allen
All rights reserved.

The Arduboy2 library was forked from the Arduboy library:
https://github.com/Arduboy/Arduboy
Copyright (c) 2016, Kevin "Arduboy" Bates
Copyright (c) 2016, Chris Martinez
Copyright (c) 2016, Josh Goebel
Copyright (c) 2016, Scott Allen
All rights reserved.
which is in turn partially based on the Adafruit_SSD1306 library
https://github.com/adafruit/Adafruit_SSD1306
Copyright (c) 2012, Adafruit Industries
All rights reserved.

This also uses code from these libraries:

* Arduboy2 library from https://github.com/MLXXXp/Arduboy2
* FixedPoint library from https://github.com/Pharap/FixedPointsArduino/
* ArduboyTones library from https://github.com/MLXXXp/ArduboyTones