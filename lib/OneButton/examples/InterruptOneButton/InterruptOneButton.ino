/*
 InterruptOneButton.ino - Example for the OneButtonLibrary library.
 This is a sample sketch to show how to use the OneButtonLibrary
 to detect double-click events on a button by using interrupts. 
 The library internals are explained at
 http://www.mathertel.de/Arduino/OneButtonLibrary.aspx

 Good article on Arduino UNO interrupts:
 https://arduino.stackexchange.com/questions/30968/how-do-interrupts-work-on-the-arduino-uno-and-similar-boards
 ... and the processor datasheet.
  
 Setup a test circuit:
 * Connect a pushbutton to the PIN_INPUT (see defines for processor specific examples) and ground.
 * The PIN_LED (see defines for processor specific examples) is used for output attach a led and resistor to VCC.
   or maybe use a built-in led on the standard arduino board.
   
 The sketch shows how to setup the library and bind the functions (singleClick, doubleClick) to the events.
 In the loop function the button.tick function must be called as often as you like.
 By using interrupts the internal state advances even when for longer time the button.tick is not called.
*/

// 03.03.2011 created by Matthias Hertel
// 01.12.2011 extension changed to work with the Arduino 1.0 environment
// 04.11.2017 Interrupt version created.
// 04.11.2017 Interrupt version using attachInterrupt.

#include "OneButton.h"

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO_EVERY)
// Example for Arduino UNO with input button on pin 2 and builtin LED on pin 13
// attachInterrupt only supports pin 2 and 3 on UNO.
// See https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
#define PIN_INPUT 2
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

// Setup a new OneButton on pin PIN_INPUT
// The 2. parameter activeLOW is true, because external wiring sets the button to LOW when pressed.
OneButton button(PIN_INPUT, true);

// current LED state, staring with LOW (0)
int ledState = LOW;

// save the millis when a press has started.
unsigned long pressStartTime;

// In case the momentary button puts the input to HIGH when pressed:
// The 2. parameter activeLOW is false when the external wiring sets the button to HIGH when pressed.
// The 3. parameter can be used to disable the PullUp .
// OneButton button(PIN_INPUT, false, false);


// This function is called from the interrupt when the signal on the PIN_INPUT has changed.
// do not use Serial in here.
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO_EVERY)
void checkTicks() {
  // include all buttons here to be checked
  button.tick(); // just call tick() to check the state.
}

#elif defined(ESP8266)
ICACHE_RAM_ATTR void checkTicks() {
  // include all buttons here to be checked
  button.tick(); // just call tick() to check the state.
}

#elif defined(ESP32)
void IRAM_ATTR checkTicks() {
  // include all buttons here to be checked
  button.tick(); // just call tick() to check the state.
}

#endif


// this function will be called when the button was pressed 1 time only.
void singleClick() {
  Serial.println("singleClick() detected.");
} // singleClick


// this function will be called when the button was pressed 2 times in a short timeframe.
void doubleClick() {
  Serial.println("doubleClick() detected.");

  ledState = !ledState; // reverse the LED
  digitalWrite(PIN_LED, ledState);
} // doubleClick


// this function will be called when the button was pressed multiple times in a short timeframe.
void multiClick() {
  int n = button.getNumberClicks();
  if (n == 3) {
    Serial.println("tripleClick detected.");
  } else if (n == 4) {
    Serial.println("quadrupleClick detected.");
  } else {
    Serial.print("multiClick(");
    Serial.print(n);
    Serial.println(") detected.");
  }

  ledState = !ledState; // reverse the LED
  digitalWrite(PIN_LED, ledState);
} // multiClick


// this function will be called when the button was held down for 1 second or more.
void pressStart() {
  Serial.println("pressStart()");
  pressStartTime = millis() - 1000; // as set in setPressTicks()
} // pressStart()


// this function will be called when the button was released after a long hold.
void pressStop() {
  Serial.print("pressStop(");
  Serial.print(millis() - pressStartTime);
  Serial.println(") detected.");
} // pressStop()


// setup code here, to run once:
void setup() {
  Serial.begin(115200);
  Serial.println("One Button Example with interrupts.");

  // enable the led output.
  pinMode(PIN_LED, OUTPUT); // sets the digital pin as output
  digitalWrite(PIN_LED, ledState);

  // setup interrupt routine
  // when not registering to the interrupt the sketch also works when the tick is called frequently.
  attachInterrupt(digitalPinToInterrupt(PIN_INPUT), checkTicks, CHANGE);

  // link the xxxclick functions to be called on xxxclick event.
  button.attachClick(singleClick);
  button.attachDoubleClick(doubleClick);
  button.attachMultiClick(multiClick);

  button.setPressTicks(1000); // that is the time when LongPressStart is called
  button.attachLongPressStart(pressStart);
  button.attachLongPressStop(pressStop);

  // A1-Option for UNO:
  // it is possible to use e.g. A1 but then some direct register modifications and an ISR has to be used:
  // You may have to modify the next 2 lines if using another pin than A1
  // PCICR |= (1 << PCIE1);   // This enables Pin Change Interrupt 1 that covers the Analog input pins or Port C.
  // PCMSK1 |= (1 << PCINT9); // This enables the interrupt for pin 1 of Port C: This is A1.

} // setup

// A1-Option for UNO:
// The Interrupt Service Routine for Pin Change Interrupt 1
// This routine will only be called on any signal change on A1: exactly where we need to check.
// ISR(PCINT1_vect)
// {
//   // keep watching the push button:
//   button.tick(); // just call tick() to check the state.
// }


// main code here, to run repeatedly:
void loop() {
  // keep watching the push button, even when no interrupt happens:
  button.tick();

  // You can implement other code in here or just wait a while
  delay(10);
} // loop


// End
