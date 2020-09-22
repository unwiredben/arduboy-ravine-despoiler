/*
   Copyright (C) 2020 Ben Combee (@unwiredben)
   Copyright (c) 2016-2020, Scott Allen
   All rights reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#pragma once

#include <Arduboy2.h>
#include <ArduboyTones.h>

#include <FixedPoints.h>
using Number = SFixed<7, 8>;
using BigNumber = SFixed<15, 16>;

#include "Util.h"

#include "boulder_bmp.h"
#include "final_cmpbmp.h"
#include "final_numbers_bmp.h"
#include "logo_bmp.h"
#include "plane_bmp.h"
#include "press_a_bmp.h"
#include "ravine_bmp.h"
#include "titlecard_cmpbmp.h"
#include "unwired_logo_bmp.h"
#include "zeppelin_bmp.h"

/* global definitions for APIs */
Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Sprites sprites;

namespace RavineDespoilerGame {

// Simulates gravity
// Earth's gravitational pull is 9.8 m/s squared
// But that's far too powerful for the tiny screen
// So I picked something small
constexpr Number CoefficientOfGravity = 0.3;

int16_t score;

struct GameObject {
  BigNumber x = 0, x_min = 0, x_max = WIDTH, x_vel = 0;
  Number y = 0, y_min = 0, y_max = HEIGHT, y_vel = 0;

  void move(BigNumber newX, Number newY) {
    x = newX;
    x = constrain(x, x_min, x_max);
    y = newY;
    y = constrain(y, y_min, y_max);
  }
  void adjust(BigNumber dx, Number dy) { move(x + dx, y + dy); }

  // for this game, we use a looping x coordinate system
  // where going off one end will move you to the other
  void applyXVelocity() {
    x = x + x_vel;
    if (x < x_min) {
      x = x_min;
      x_vel = -x_vel;
    } else if (x > x_max) {
      x = x_max;
      x_vel = -x_vel;
    }
  }
};

struct Ravine {
  static constexpr uint8_t boulders_width = 21;
  static constexpr uint8_t boulders_height = 8;
  static char boulders[boulders_height][boulders_width];

  static constexpr uint8_t boulders_x_start = 11;
  static constexpr uint8_t boulders_y_start = 0 + ravine_top;
  static constexpr uint8_t boulder_size = 5;

  uint16_t boulder_count;

  bool cleared() { return boulder_count == 0; }

  void explodeAt(uint8_t bx, uint8_t by) {
    char &b = boulders[by][bx];
    if (b == '0') {
      b = '1';
      --boulder_count;
      // score one point for each boulder above & including the one being
      // destroyed
      for (int8_t i = by; i >= 0; --i) {
        if (boulders[i][bx] != ' ')
          ++score;
      }
    }
  }

  void reset(char fill) {
    // reset all non-walls to supplied state
    boulder_count = 0;
    for (int8_t i = 0; i < boulders_width; ++i) {
      for (int8_t j = 0; j < boulders_height; ++j) {
        char b = boulders[j][i];
        if (b != 'W') {
          boulders[j][i] = fill;
          ++boulder_count;
        }
      }
    }
  }

  void update() {
    // first update explosions
    for (int8_t i = 0; i < boulders_width; ++i) {
      for (int8_t j = 0; j < boulders_height; ++j) {
        char &b = boulders[j][i];
        if (b >= '1' && b <= '5')
          b = b + 1;
        else if (b == '6')
          b = ' ';
      }
    }

    // then update boulders to account for gravity.
    // start at next-to-last row and search up to have a full
    // column fall together
    for (int8_t i = 0; i < boulders_width; ++i) {
      for (int8_t j = boulders_height - 2; j >= 0; --j) {
        char &b1 = boulders[j][i];
        char &b0 = boulders[j + 1][i];
        if (b0 == ' ') {
          if (b1 == '0') {
            b1 = '.';
          } else if (b1 == '.') {
            b1 = ' ';
            b0 = '0';
          }
        } else if (b0 == '.' && b1 == '0') {
          b1 = '.';
        }
      }
    }
  }

  void draw() {
    sprites.drawOverwrite(0, ravine_top, ravine_bmp, 0);
    for (int8_t i = 0; i < boulders_width; ++i) {
      for (int8_t j = 0; j < boulders_height; ++j) {
        char b = boulders[j][i];
        if (b >= '0' && b <= '6') {
          sprites.drawPlusMask(i * boulder_size + boulders_x_start,
                               j * boulder_size + boulders_y_start,
                               boulder_plus_mask, b - '0');
        } else if (b == '.') {
          sprites.drawPlusMask(i * boulder_size + boulders_x_start,
                               j * boulder_size + boulders_y_start + 3,
                               boulder_plus_mask, 0);
        }
      }
    }
  }

  // maps screen coordinate to boulder number, returns true if boulder
  // coordinates are valid
  bool boulderAtXY(uint8_t x, uint8_t y, uint8_t &bx, uint8_t &by) {
    bx = (x - boulders_x_start) / boulder_size;
    by = (y - boulders_y_start) / boulder_size;
    return (bx < boulders_width) && (by < boulders_height);
  }
} // namespace RavineDespoilerGame
ravine;

struct Bomb : public GameObject {
  static constexpr uint8_t framesToKeepFalling = 3;

  bool active;
  uint8_t postCollisionFrames;

  void miss() {
    active = false;
    // penalty for missing!
    if (!postCollisionFrames)
      score = max(0, score - 5);
  }

  void checkForCollision(uint8_t ix, uint8_t iy) {
    // check if we've gone off the screen
    if (y >= HEIGHT || x >= WIDTH) {
      miss();
      return;
    }

    if (arduboy.getPixel(ix, iy) == WHITE) {
      uint8_t bx, by;
      if (ravine.boulderAtXY(ix, iy, bx, by)) {
        char boulder = ravine.boulders[by][bx];
        if (boulder == 'W') {
          miss();
        } else if (boulder == '0') {
          if (postCollisionFrames == 0) {
            postCollisionFrames = framesToKeepFalling;
          }
          ravine.explodeAt(bx, by);
        }
      }
    }
  }

  void applyVelocity() {
    // once we collide, bomb only stays active for a few frames allowing
    // pentrating a few boulders, then it disappears
    if (postCollisionFrames && !--postCollisionFrames) {
      active = false;
    }
    if (!active)
      return;

    auto x0 = x.getInteger();
    auto y0 = y.getInteger();
    x = x + x_vel;
    y = y + y_vel;
    y_vel = y_vel + CoefficientOfGravity;
    auto x1 = x.getInteger();
    auto y1 = y.getInteger();

    // adaptation of Bresenham's Line Drawing algorithm
    // from Arduboy2 library

    bool steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
      swap(x0, y0);
      swap(x1, y1);
    }

    if (x0 > x1) {
      swap(x0, x1);
      swap(y0, y1);
    }

    int16_t dx = x1 - x0;
    int16_t dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int8_t ystep;

    if (y0 < y1) {
      ystep = 1;
    } else {
      ystep = -1;
    }

    for (; x0 <= x1; x0++) {
      if (steep) {
        checkForCollision(y0, x0);
        if (!active)
          return;
      } else {
        checkForCollision(x0, y0);
        if (!active)
          return;
      }

      err -= dy;
      if (err < 0) {
        y0 += ystep;
        err += dx;
      }
    }
  }

  void draw() {
    if (active) {
      int8_t ix = x.getInteger(), iy = y.getInteger();
      arduboy.drawPixel(ix, iy, WHITE);
    }
  }

  void reset() {
    active = false;
    postCollisionFrames = 0;
  }

  void drop(BigNumber x, Number y, BigNumber x_vel) {
    active = true;
    this->x = x;
    this->y = y;
    y_vel = 0;
    this->x_vel = x_vel;
  }
} bomb;

struct Plane : public GameObject {
  static constexpr BigNumber offscreen_x_margin = 16;

  Plane() {
    x_min = -offscreen_x_margin - plane_width;
    x_max = WIDTH + offscreen_x_margin;
    y_min = 0;
    y_max = ravine_top - plane_height - 2;
  }

  void applyXVelocity() {
    GameObject::applyXVelocity();
    if (x == x_min || x == x_max) {
      y = randomSFixed(y_min, y_max);
    }
  }

  void reset() {
    x = x_min;
    y = 2;
    x_vel = 1;
  }

  void draw() {
    int8_t ix = x.getInteger(), iy = y.getInteger();
    sprites.drawPlusMask(ix, iy, plane_plus_mask, x_vel > 0);
  }

  bool visible() { return (x > -plane_width) && (x < WIDTH); }
} plane;

struct Zeppelin : public GameObject {
  static constexpr BigNumber offscreen_x_margin = 20;
  Zeppelin() {
    x_min = -offscreen_x_margin - plane_width;
    x_max = WIDTH + offscreen_x_margin;
    y_min = y_max = 0;
  }

  void reset() {
    x = x_max;
    y = 0;
    x_vel = -0.5;
  }

  void draw() {
    int8_t ix = x.getInteger(), iy = y.getInteger();
    sprites.drawPlusMask(ix, iy, zeppelin_plus_mask, x_vel > 0);
  }
} zeppelin;

// coordinating game state
enum GameState {
  INITIAL_LOGO,
  TITLE_SCREEN,
  OBJECTIVE_SCREEN,
  GAME_ACTIVE,
  LEVEL_COMPLETE
} state = INITIAL_LOGO;

void enter_state(GameState newState) {
  arduboy.frameCount = 0;
  sound.noTone();
  state = newState;

  if (newState == TITLE_SCREEN) {
    // reset game state for a new game
    zeppelin.reset();
    ravine.reset(' ');
  } else if (newState == GAME_ACTIVE) {
    // reset UI state for a new level
    randomSeed(arduboy.generateRandomSeed());
    plane.reset();
    ravine.reset('0');
    score = 0;
  }
}

void initial_logo() {
  if (arduboy.frameCount == 1) {
    arduboy.clear();
    arduboy.drawCompressed(0, 0, unwiredgames_logo_cmpimg);
  }
  if (arduboy.frameCount > 90) {
    enter_state(TITLE_SCREEN);
  }
}

void title_screen() {
  if (arduboy.frameCount == 1) {
    arduboy.clear();
    ravine.draw();
    sprites.drawOverwrite(logo_x, logo_y, logo_bmp, 0);
  }
  if (arduboy.frameCount == 180) {
    arduboy.fillRect(24, 48, press_a_to_start_width, press_a_to_start_height,
                     BLACK);
    arduboy.drawCompressed(24, 48, press_a_to_start_cmpbmp);
  }

  arduboy.fillRect(zeppelin.x.getInteger(), zeppelin.y.getInteger(),
                   zeppelin_width, zeppelin_height, BLACK);
  zeppelin.applyXVelocity();
  zeppelin.draw();

  if (arduboy.justPressed(A_BUTTON)) {
    // enter_state(OBJECTIVE_SCREEN);
    enter_state(GAME_ACTIVE);
  }
}

void objective_screen() {
  if (arduboy.frameCount == 1) {
    arduboy.clear();
    // arduboy.drawCompressed(0, 0, objective_cmpimg);
  }
  if (arduboy.frameCount > 120) {
    enter_state(GAME_ACTIVE);
  }
}

void drawScore(uint8_t x, uint8_t y) {
  auto s = score;
  x = x + final_numbers_width * 2;
  do {
    sprites.drawOverwrite(x, y, final_numbers_bmp, s % 10);
    s = s / 10;
    x = x - final_numbers_height;
  } while (s != 0);
}

void game_active() {
  // process input
  plane.x_vel = plane.x_vel < 0 ? -1 : 1;
  if (arduboy.pressed(LEFT_BUTTON)) {
    plane.x_vel = plane.x_vel < 0 ? -1.5 : 0.5;
  }
  if (arduboy.pressed(RIGHT_BUTTON)) {
    plane.x_vel = plane.x_vel < 0 ? -0.5 : 1.5;
  }
  plane.applyXVelocity();

  if (arduboy.justPressed(A_BUTTON) && !bomb.active && plane.visible()) {
    bomb.drop(plane.x + (plane_width / 2), plane.y + plane_height, plane.x_vel);
  }
  bomb.applyVelocity();

  // update boulders every four frames
  if (arduboy.frameCount % 4 == 0) {
    ravine.update();
  }

  // if (arduboy.frameCount % 128 == 0) {
  //   ravine.explodeOne();
  // }

  arduboy.clear();
  ravine.draw();
  if (plane.visible()) {
    plane.draw();
  } else {
    if (ravine.cleared()) {
      enter_state(GameState::LEVEL_COMPLETE);
    } else {
      drawScore(center_x(3 * final_numbers_width), 0);
    }
  }
  bomb.draw();

  // temporary change to allow resetting
  if (arduboy.pressed(A_BUTTON) && arduboy.justPressed(B_BUTTON)) {
    enter_state(INITIAL_LOGO);
  }
}

void level_complete() {
  if (arduboy.frameCount == 10) {
    uint8_t x = center_x(final_width);
    uint8_t y = center_y(final_height);
    arduboy.fillRect(x - 1, y, final_width + 2, final_height, BLACK);
    arduboy.drawCompressed(center_x(final_width), center_y(final_height),
                           final_cmpbmp);
    drawScore(x + final_numbers_x, y + final_numbers_y);
  }

  if (arduboy.frameCount > 120 && !sound.playing() &&
      arduboy.pressed(A_BUTTON)) {
    enter_state(TITLE_SCREEN);
  }
}

void setup(void) {
  arduboy.begin();
  arduboy.setFrameRate(60);
  enter_state(INITIAL_LOGO);
}

void loop(void) {
  if (!(arduboy.nextFrameDEV()))
    // if (!(arduboy.nextFrame()))
    return;

  arduboy.pollButtons();

  switch (state) {
  case INITIAL_LOGO:
    initial_logo();
    break;
  case TITLE_SCREEN:
    title_screen();
    break;
  case OBJECTIVE_SCREEN:
    objective_screen();
    break;
  case GAME_ACTIVE:
    game_active();
    break;
  case LEVEL_COMPLETE:
    level_complete();
    break;
  }

  arduboy.display();
}

// values are '0' through '5' for boulder.  0 is the normal state, while 1..5
// are various states of the explosion. ' ' stands for no boulder, while 'W'
// indicates that there is the wall of the ravine.  State '.' is a boulder
// falling to the hole below. Initial settings are just holes and walls, as
// holes are initially all reset to boulders at the start of a run.

// clang-format off
char Ravine::boulders[boulders_height][boulders_width] = {
    { ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', ' ' },
    { ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', ' ' },
    { ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', ' ' },
    { ' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', 'W' },
    { 'W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ', 'W' },
    { 'W',' ',' ',' ',' ',' ',' ',' ',' ','W','W',' ',' ',' ',' ',' ',' ',' ',' ',' ', 'W' },
    { 'W',' ',' ',' ',' ',' ',' ',' ','W','W','W','W',' ',' ',' ',' ',' ',' ',' ',' ', 'W' },
    { 'W',' ',' ',' ',' ',' ',' ',' ','W','W','W','W','W',' ',' ',' ',' ',' ',' ','W', 'W' },
};
// clang-format on

} // namespace RavineDespoilerGame
