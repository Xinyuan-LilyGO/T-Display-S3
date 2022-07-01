/*
 BlinkMachine.ino
 
 This is a sample sketch to show how to use the OneButtonLibrary to detect double-click events on a button. 
 
 Copyright (c) by Matthias Hertel, http://www.mathertel.de
 This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
 More information on: http://www.mathertel.de/Arduino
 
 The library internals are explained at
 http://www.mathertel.de/Arduino/OneButtonLibrary.aspx
 
 Setup a test circuit:
 * Connect a pushbutton to pin A1 (Uno) or D3 (ESP8266) and ground.
 * The pin 13 (UNO) or D4 (ESP8266) is used for output attach a led and resistor to ground
 or see the built-in led on the standard arduino board.
 
 The Sketch shows how to setup the library and bind a "machine" that can blink the LED slow or fast.
 A click on the button turns the led on.
 A doubleclick on the button changes the blink rate from ON to SLOW to FAST and back.
 In the loop function the button.tick function has to be called as often as you like.

 State-Diagram

    start
      |                   +-------\
      V                   V       |
  --------              ------    |
 |  OFF   |<--click-+->|  ON  |   |
  --------          |   ------    |
                    |     |       |
                    |   d-click   |
                    |     |       |
                    |     V       |
                    |   ------    |
                    +- | SLOW |   |
                    |   ------    |
                    |     |       |
                    |   d-click   |
                    |     |       |
                    |     V    d-click
                    |   ------    |
                    +--| FAST |---/
                        ------ 
 */

// 06.10.2012 created by Matthias Hertel
// 26.03.2017 state diagram added, minor changes

#include "OneButton.h"

// The actions I ca do...
typedef enum {
  ACTION_OFF,  // set LED "OFF".
  ACTION_ON,   // set LED "ON"
  ACTION_SLOW, // blink LED "SLOW"
  ACTION_FAST  // blink LED "FAST"
} 
MyActions;

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO_EVERY)
// Example for Arduino UNO with input button on pin 2 and builtin LED on pin 13
#define PIN_INPUT A1
#define PIN_LED 13

#elif defined(ESP8266)
// Example for NodeMCU with input button using FLASH button on D3 and using the led on -12 module (D4).
// This LED is lighting on output level LOW.
#define PIN_INPUT D3
#define PIN_LED D4

#elif defined(ESP32)
// Example pin assignments for a ESP32 board
// Some boards have a BOOT switch using GPIO 0.
#define PIN_INPUT 0
// Attach a LED using GPIO 25 and VCC. The LED is on when output level is LOW.
#define PIN_LED 25

#endif

// Setup a new OneButton on pin PIN_INPUT.  
OneButton button(PIN_INPUT, true);

MyActions nextAction = ACTION_OFF; // no action when starting


// setup code here, to run once.
void setup() {
  // enable the standard led on pin 13.
  pinMode(PIN_LED, OUTPUT);      // sets the digital pin as output

  // link the myClickFunction function to be called on a click event.   
  button.attachClick(myClickFunction);

  // link the doubleclick function to be called on a doubleclick event.   
  button.attachDoubleClick(myDoubleClickFunction);

  // set 80 msec. debouncing time. Default is 50 msec.
  button.setDebounceTicks(80);
} // setup


// main code here, to run repeatedly: 
void loop() {
  unsigned long now = millis();

  // keep watching the push button:
  button.tick();

  // You can implement other code in here or just wait a while 

  if (nextAction == ACTION_OFF) {
    // do nothing.
    digitalWrite(PIN_LED, LOW);

  } else if (nextAction == ACTION_ON) {
    // turn LED on
    digitalWrite(PIN_LED, HIGH);

  } else if (nextAction == ACTION_SLOW) {
    // do a slow blinking
    if (now % 1000 < 500) {
      digitalWrite(PIN_LED, LOW);
    } else {
      digitalWrite(PIN_LED, HIGH);
    } // if

  } else if (nextAction == ACTION_FAST) {
    // do a fast blinking
    if (now % 200 < 100) {
      digitalWrite(PIN_LED, LOW);
    } else {
      digitalWrite(PIN_LED, HIGH);
    } // if
  } // if
} // loop


// this function will be called when the button was pressed 1 time and them some time has passed.
void myClickFunction() {
  if (nextAction == ACTION_OFF)
    nextAction = ACTION_ON;
  else
    nextAction = ACTION_OFF;
} // myClickFunction


// this function will be called when the button was pressed 2 times in a short timeframe.
void myDoubleClickFunction() {
  if (nextAction == ACTION_ON) {
    nextAction = ACTION_SLOW;

  } else if (nextAction == ACTION_SLOW) {
    nextAction = ACTION_FAST;

  } else if (nextAction == ACTION_FAST) {
    nextAction = ACTION_ON;
  } // if
} // myDoubleClickFunction

// End

