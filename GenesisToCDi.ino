
/*******************************
 *
 *  File: GenesisToCDi.ino
 *  Description: software for the GenesisToCDi gamepad converter (allows to use a 3 or 6 button Genesis gamepad on a Philips CDi)
 *  Author: 
 *  Date: 01-08-2022
 *  Version: 0.2
 *  Thanks: Rob Duarte (SNESpad library), Paul Hackmann (documentation about the CDi pointing devices), Laurent Berta (SNEStoCDi)
 *  License: CC-BY 4.0 ( http://creativecommons.org/licenses/by/4.0/legalcode )
 *
 ******************************/

#include <SoftwareSerial.h>
//#include "SNESpad.h"
#include "Genesispad.h"
#include <EEPROM.h>

//function declarations
bool assertRTS();
int adjustSpeed(int val);
void changeSpeed(byte newspeed);

//flag declarations
//various controller mapping options
static const uint8_t mapping3button   =     0x01;
static const uint8_t mapping4button   =     0x02;  //default if there is no match
static const uint8_t mappingLinkZelda =     0x03;  //sword = button 1 = B, item = button 2 = C, jump = up = A (not implemented)
static const uint8_t mappingMario     =     0x04;  //because for Mario, from the beginning of time, the right most button has always been "jump"

//SNESpad pad = SNESpad(5, 6, 7); // Create a SNESpad instance, change the pin values to match your wiring (latch, clock, data)
Genesispad pad = Genesispad(2, A0, A1, A2, A3, A4, A5);   // Create an Genesispad instance.  Can't seem to use A6 for PIN_SELECT.
SoftwareSerial vSerial(9, 10, true); // RX, TX, inverse_logic. RX is not used here, as the CDi only communicates on the RTS line
const int RTSpin = A7; // the number of the analog pin used to receive the RTS signal from the CDi
const int ledPin = 13; // the number of the inboard LED pin

const int RTSthreshold = 328; // threshold for the CDi RTS analog detection
uint16_t btns;
bool btnMODEpressed = false;
bool btnSTARTpressed = false;
int padbyte0, padbyte1, padbyte2, oldpadbyte0, oldpadbyte1, oldpadbyte2, x, y;
byte spd;
bool firstId = true;
bool btnLpressed = false;
bool btnSEpressed = false;
uint8_t controllerMapping = mapping4button;  //I have a 4-button controller, so that's what I am used to.

// init
void setup()
{
  oldpadbyte0 = 0;
  oldpadbyte1 = 0;
  oldpadbyte2 = 0;

  byte eepromData = EEPROM.read(0); // retrieve speed setting from the Arduino's EEPROM
  if(eepromData >= 1 && eepromData <= 5) spd = eepromData;
  else spd = 2;
  
  pinMode(ledPin, OUTPUT);
  vSerial.begin(1200); // open serial interface to send data to the CDi
}

// main
void loop()
{
  bool goodsend = false;
  if(!assertRTS()) {
    digitalWrite(ledPin, HIGH);
    while(!assertRTS()) { } // wait for CDi to assert the RTS line
    if(firstId) delay(100);
    else delay(1);
    firstId = false;
    vSerial.write(0b11001010); // send device id ("maneuvering device")
  }
  digitalWrite(ledPin, LOW);

	// Get the state of the Genesis pad buttons
  pad.pollbuttons();
  pad.pollbuttons();      //seems the 6-button controller needs repetition to lock on.

  // manage speed control
  if(pad.status3button & GENESIS_START) {
    //use the start button to cycle through the 5 speed settings.  Mode resets to 2nd speed.
    if(!btnSTARTpressed) {
      changeSpeed(spd+1); // speed : up
      btnSTARTpressed = true;
    }
  } else {
    btnSTARTpressed = false;
  }
/*  if(btns & SNES_L) {
    if(!btnLpressed) changeSpeed(spd-1); // speed : down
    btnLpressed = true;
  }
  else btnLpressed = false;*/

  //Reset speed to a normally useful value (proved to be too easy to push by accident)
/*  if(pad.status6button & GENESIS_MODE) {
    if(!btnMODEpressed) {
      changeSpeed(2); // speed : default (2)
    }
    btnMODEpressed = true;
  } else {
    btnMODEpressed = false;
  }*/

  padbyte0 = 0b11000000;  //initialize data bytes
  padbyte1 = 0b10000000;
  padbyte2 = 0b10000000;

  // Dpad X axis
  x = 127;
  if(pad.status3button & GENESIS_LEFT) {
    x = 254;
  }
  if(pad.status3button & GENESIS_RIGHT) {
    x = 1;
  }
  x = adjustSpeed(x);
  
  if(x<127) {
    // right
    x = x ^ 0b01111111;
    x = x + 1;
    padbyte1 = padbyte1 | x;
    padbyte1 = padbyte1 & 0b10111111;
    if((x & 0b01000000) != 0)
      padbyte0 = padbyte0 | 0b00000001;
  }
  else if(x>127) {
    // left
    x = x ^ 0b01111111;
    x = x + 1;
    padbyte1 = padbyte1 | x;
    padbyte1 = padbyte1 & 0b10111111;
    if((x & 0b01000000) != 0)
      padbyte0 = padbyte0 | 0b00000011;
    else
      padbyte0 = padbyte0 | 0b00000010; 
  }

  // Dpad Y axis (and all top buttons because 'up' to jump is a pain).  To use top row for custom mapping, remove them from here first.
  y = 127;
  if((pad.status3button & GENESIS_UP) || (pad.status6button & GENESIS_X) || (pad.status6button & GENESIS_Y) || (pad.status6button & GENESIS_Z)) {
    y = 254;
  }
  if(pad.status3button & GENESIS_DOWN) {
    y = 1;
  }
  y = adjustSpeed(y);

  if(y<127) // down
  {
    y = y ^ 0b01111111;
    y = y + 1;
    padbyte2 = padbyte2 | y;
    padbyte2 = padbyte2 & 0b10111111;
    if((y & 0b01000000) != 0)
      padbyte0 = padbyte0 | 0b00000100;
  }
  else if(y>127) // up
  {
    y = y ^ 0b01111111;
    y = y + 1;
    padbyte2 = padbyte2 | y;
    padbyte2 = padbyte2 & 0b10111111;
    if((y & 0b01000000) != 0)
      padbyte0 = padbyte0 | 0b00001100;
    else
      padbyte0 = padbyte0 | 0b00001000;
  }

  // buttons
  //if(btns & SNES_SELECT) {
  //  if(!btnSEpressed) standardMapping = !standardMapping; // mapping change : invert buttons 1 & 2 (Y & B)
  //  btnSEpressed = true;
  //}
  //else btnSEpressed = false;
  if(controllerMapping == mapping3button) {
    //map like 3-button CD-i pad
    if(pad.status3button & GENESIS_A) padbyte0 = padbyte0 | 0b00100000;  //button 1 (A)
    if(pad.status3button & GENESIS_B) padbyte0 = padbyte0 | 0b00010000;  //button 2 (B)
    if(pad.status3button & GENESIS_C) padbyte0 = padbyte0 | 0b00110000; // button 3 aka 1&2 together (C)
  } else if(controllerMapping == mappingLinkZelda) {
    //sword = button 1 = B, item = button 2 = C, jump = up = A (not implemented)
    if(pad.status3button & GENESIS_B) padbyte0 = padbyte0 | 0b00100000;  //button 1 (A)
    if(pad.status3button & GENESIS_C) padbyte0 = padbyte0 | 0b00010000;  //button 2 (B)
    if(pad.status3button & GENESIS_A) padbyte0 = padbyte0 | 0b00110000; // button 3 aka 1&2 together (C) //I really want to map this to "UP"
  } else if(controllerMapping == mappingMario) {
    //because for Mario, from the beginning of time, the right most button has always been "jump"
    if(pad.status3button & GENESIS_C) padbyte0 = padbyte0 | 0b00100000;  //button 1 (A)
    if(pad.status3button & GENESIS_B) padbyte0 = padbyte0 | 0b00010000;  //button 2 (B)
    if(pad.status3button & GENESIS_A) padbyte0 = padbyte0 | 0b00110000; // button 3 aka 1&2 together (C)
  } else /*if(controllerMapping == mapping4button)*/ {
    //map like 4-button CD-i pad - I made this the default
    if(pad.status3button & GENESIS_B) padbyte0 = padbyte0 | 0b00100000;  //button 1 (A)
    if(pad.status3button & GENESIS_C) padbyte0 = padbyte0 | 0b00010000;  //button 2 (B)
    if(pad.status3button & GENESIS_A) padbyte0 = padbyte0 | 0b00110000; // button 3 aka 1&2 together (C)
  }

  if((padbyte0 != oldpadbyte0) || (padbyte1 != 0b10000000) || (padbyte2 != 0b10000000) || ((padbyte0 & 0b00001111) != 0))  // see if state has changed
  {
    //Ah, if !assertRTS() during send, the rest of the send is lost, and save state prevents resend.
    //Make sure all three bytes send before saving to oldpadbyteX
    if(assertRTS()) goodsend = false;
    if(assertRTS()) {
      vSerial.write(padbyte0);
      if(assertRTS()) {
        vSerial.write(padbyte1);
        if(assertRTS()) {
          vSerial.write(padbyte2);
          goodsend = true;
        }
      }
    }
  }

  // save state if sent
  if(goodsend) {
    oldpadbyte0 = padbyte0; 
    oldpadbyte1 = padbyte1;
    oldpadbyte2 = padbyte2;
  }
}

// true if RTS asserted
bool assertRTS() {
  if(analogRead(RTSpin) < RTSthreshold) return false;
  else return true;
}

// send back the correct Dpad value depending on the speed setting
int adjustSpeed(int val)
{
  if(val==127 || spd==5) return val;

  if(val==254) {
    if(spd==4) return 202;
    if(spd==3) return 170;
    if(spd==2) return 149;
    if(spd==1) return 130;
  }
  else {
    if(spd==4) return 53;
    if(spd==3) return 85;
    if(spd==2) return 106;
    if(spd==1) return 125;
  }
}

// change speed setting and save it to the EEPROM
void changeSpeed(byte newspeed)
{
  //if(newspeed<1) newspeed=1;
  //else if(newspeed>5) newspeed=5;
  if(newspeed<1) newspeed=1;
  else if(newspeed>5) newspeed=1;
  
  spd=newspeed;
  EEPROM.write(0, spd);
}
