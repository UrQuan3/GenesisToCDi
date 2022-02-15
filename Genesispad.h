/*
  Genesispad - Arduino library for interfacing with a Genesis joystick

  Version: 1.0 (01/07/2022) - Created (Brian Harrison)

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITSNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Styling based off of the SNESpad library
// Why can't I use A6 for PIN_SELECT?

#ifndef Genesispad_h
#define Genesispad_h

#include <inttypes.h>

static const int cycledelay = 200;

static const uint8_t GENESIS_UP =     0x01;
static const uint8_t GENESIS_DOWN =   0x02;
static const uint8_t GENESIS_LEFT =   0x04;
static const uint8_t GENESIS_RIGHT =  0x08;
static const uint8_t GENESIS_A =      0x10;
static const uint8_t GENESIS_B =      0x20;
static const uint8_t GENESIS_C =      0x40;
static const uint8_t GENESIS_START =  0x80;

static const uint8_t GENESIS_Z =      0x01;
static const uint8_t GENESIS_Y =      0x02;
static const uint8_t GENESIS_X =      0x04;
static const uint8_t GENESIS_MODE =   0x08;

class Genesispad {

public:
    //Genesispad();
    Genesispad(uint8_t PIN_SELECT, uint8_t PIN_IN_UP, uint8_t PIN_IN_DOWN, uint8_t PIN_IN_LEFT, uint8_t PIN_IN_RIGHT, uint8_t PIN_IN_B, uint8_t PIN_IN_C);
    uint8_t status3button;
    uint8_t status6button;
    void pollbuttons();

private:
    uint8_t p_select, p_up, p_down, p_left, p_right, p_B, p_C;
    uint8_t statuspins = 0b00000000;
    int select = 0;

    //Above 200 causes failure of 6-button mode.
    //However 20 works, and cuts latencies needed to support 6-button controllers down to 120 microseconds.
    static const int cycledelay = 20;
    
};

#endif
