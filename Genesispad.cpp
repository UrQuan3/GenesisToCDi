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

#include "Genesispad.h"
#include "Arduino.h"

//Above 200 causes failure of 6-button mode.
//200 shows there is not much margin as 500+ should work.

Genesispad::Genesispad(uint8_t PIN_SELECT, uint8_t PIN_IN_UP, uint8_t PIN_IN_DOWN, uint8_t PIN_IN_LEFT, uint8_t PIN_IN_RIGHT, uint8_t PIN_IN_B, uint8_t PIN_IN_C)
: p_select(PIN_SELECT),
p_up(PIN_IN_UP),
p_down(PIN_IN_DOWN),
p_left(PIN_IN_LEFT),
p_right(PIN_IN_RIGHT),
p_B(PIN_IN_B),
p_C(PIN_IN_C)
{
	pinMode(p_select, OUTPUT);
	pinMode(p_up, INPUT_PULLUP);
	pinMode(p_down, INPUT_PULLUP);
	pinMode(p_left, INPUT_PULLUP);
	pinMode(p_right, INPUT_PULLUP);
	pinMode(p_C, INPUT_PULLUP);
	pinMode(p_B, INPUT_PULLUP);

	status3button = 0b00000000;
	status6button = 0b00000000;
}

void Genesispad::pollbuttons()
{
	//
  // controller select line state
    int select = 0;

    //reset status, just in case
    statuspins = 0;

    // start with select pin high and let things settle
    select = 1;
    digitalWrite(p_select, select);
    //delay(10);
    delayMicroseconds(cycledelay);       //(0.2ms * 2) < 1.5ms reset of the 6-button controller

    /*
    Pin  Select: low  Select: high  Select: pulse-3
    --- ------------  ------------  ---------------
      1  joypad up     joypad up     button Z
      2  joypad down   joypad down   button Y
      3  logic low     joypad left   button X
      4  logic low     joypad right
      6  button A      button B
      9  start button  button C
    */

    //read the pins
    // D0   (Up)
    if(!digitalRead(p_up)) {
        statuspins |= 0b00000001;
    } else {
        statuspins &= 0b11111110;
    }

    // D1   (Down)
    if(!digitalRead(p_down)) {
        statuspins |= 0b00000010;
    } else {
        statuspins &= 0b11111101;
    }

    // D2   (Left)
    if(!digitalRead(p_left)) {
        statuspins |= 0b00000100;
    } else {
        statuspins &= 0b11111011;
    }

    // D3   (Right)
    if(!digitalRead(p_right)) {
        statuspins |= 0b00001000;
    } else {
        statuspins &= 0b11110111;
    }

    // TL   (B)
    if(!digitalRead(p_B)) {
        statuspins |= 0b00100000;
    } else {
        statuspins &= 0b11011111;
    }

    // TR   (C)
    if(!digitalRead(p_C)) {
        statuspins |= 0b01000000;
    } else {
        statuspins &= 0b10111111;
    }

    select = 0;
    digitalWrite(p_select, select);
    //delay(10);
    delayMicroseconds(cycledelay);

    // TL   (A)
    if(!digitalRead(p_B)) {
        statuspins |= 0b00010000;
    } else {
        statuspins &= 0b11101111;
    }

    // TR   (Start)
    if(!digitalRead(p_C)) {
        statuspins |= 0b10000000;
    } else {
        statuspins &= 0b01111111;
    }

    //save keys for 3-button
    status3button = statuspins;

    //special case, 6-button mode
    if(!digitalRead(p_up) && !digitalRead(p_down)) {    // if both up and down "pressed"

      //reset statuspins.  Actually need to do this here.
        statuspins = 0;

        select = 1;
        digitalWrite(p_select, select);
        //delay(10);
        delayMicroseconds(cycledelay);

        //read the pins
        // D0   (Z)
        if(!digitalRead(p_up)) {
            statuspins |= 0b00000001;
        } else {
            statuspins &= 0b11111110;
        }

        // D1   (Y)
        if(!digitalRead(p_down)) {
            statuspins |= 0b00000010;
        } else {
            statuspins &= 0b11111101;
        }

        // D2   (X)
        if(!digitalRead(p_left)) {
            statuspins |= 0b00000100;
        } else {
            statuspins &= 0b11111011;
        }

        // D3   (Mode)
        if(!digitalRead(p_right)) {
            statuspins |= 0b00001000;
        } else {
            statuspins &= 0b11110111;
        }

        status6button = statuspins;

        //trash last cycle
        select = 0;
        digitalWrite(p_select, select);
        //delay(10);
        delayMicroseconds(cycledelay);

        //test reading
        //print_state(&status3button, &status6button);
    }

    //test reading (here, it causes major delay in timing, messes up 6-button)
    //print_state(status3button, status6button);
}
