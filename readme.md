# Ravine Despoiler

a Arduboy game developed by Ben Combee

<img src="screenshot.png" width="512" height="256" alt="screenshot of the game title screen">

This game was originally started for the black & white Pebble watch.  I ended
up getting some basic animation going and had it as a watch face, but I never
implemented the game controls.  After doing my first Arduboy game jam
(which required all new programming), I figured I'd come back to this one,
porting over some of my Pebble code and assets to the new platform.

It's based on the Atari arcade game Canyon Bomber.  You can speed up or slow
down your plane and drop bombs in the canyon to knock out boulders for
points. Missing rocks will result in losing points, while hitting multiple
boulders at once gives extra points.  The game is over when the canyon is
emptied.  I'm not sure what the theoretical high score is, but my best games
have been around 240 with several misses.

Discussion of the game is at <a
href="https://community.arduboy.com/t/ravine-despoiler/9284">this Arduboy
Community thread.</a>

<a
href="https://felipemanga.github.io/ProjectABE/?url=https://raw.githubusercontent.com/unwiredben/arduboy-ravine-despoiler/main/RavineDespoiler.hex&skin=arduboy">Play
it in your browser using ProjectABE!</a>

There are two main branches in GitHub.  The "main" branch is setup for
building in the Arduino IDE, while the "main-platformio" branch is setup for
building in the PlatformIO system.

## LICENSE

Copyright (C) 2020-2022, Ben Combee \
Released under Apache 2.0 license

Physics code originally adapted from
https://github.com/Pharap/PhysixArduboy/blob/master/LICENSE used under Apache
2.0 license \
Copyright (C) 2018 Pharap (@Pharap)

The code incorporates an adaptation of the DrawLine routine from the Arduboy2
library, used under the BSD 3-part license

The code also uses an abbreviated form of the Font4x6.h header \
Copyright (c) 2019, Press Play On Tape \
All rights reserved.

Arduboy2 library: \
Copyright (c) 2016-2020, Scott Allen \
All rights reserved.

The Arduboy2 library was forked from the Arduboy library: \
https://github.com/Arduboy/Arduboy \
Copyright (c) 2016, Kevin "Arduboy" Bates \
Copyright (c) 2016, Chris Martinez \
Copyright (c) 2016, Josh Goebel \
Copyright (c) 2016, Scott Allen \
All rights reserved. \
which is in turn partially based on the Adafruit_SSD1306 library \
https://github.com/adafruit/Adafruit_SSD1306 \
Copyright (c) 2012, Adafruit Industries \
All rights reserved.

This also uses code from these libraries:

* Arduboy2 library from https://github.com/MLXXXp/Arduboy2
* FixedPoint library from https://github.com/Pharap/FixedPointsArduino/
* ArduboyTones library from https://github.com/MLXXXp/ArduboyTones