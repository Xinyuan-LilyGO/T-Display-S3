/*
 * SpecialInput.ino - Example for the OneButtonLibrary library.
 * This is a sample sketch to show how to use the OneClick library on other input sources than standard digital pins.
 *
 * The library internals are explained at
 * http://www.mathertel.de/Arduino/OneButtonLibrary.aspx
 *
 * Setup a test circuit:
 * * Connect a pushbutton to pin 2 (ButtonPin) and ground.
 *   
 * The sketch shows how to setup the library and bind the functions (singleClick, doubleClick) to the events.
 * In the loop function the button.tick function must be called as often as you like.
 *
 * * 22.01.2021 created by Matthias Hertel
*/

#include "OneButton.h"

// This is an example on how to use the OneClick library on other input sources than standard digital pins.
// 1. do not use a pin in the initialization of the OneClick library.
// 2. pass the input state to the tick function.

// You can also find how to create an instance in setup and not by declaration.
// You can also find how to use inline callback functions.

#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO_EVERY)
#define PIN_INPUT 2

#else if defined(ESP8266)
#define PIN_INPUT D3

#endif


// OneButton instance will be created in setup.
OneButton *button;

void fClicked(void *s)
{
  Serial.print("Click:");
  Serial.println((char *)s);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("One Button Example with custom input.");

// create the OneButton instance without a pin.
  button = new OneButton();

  // Here is an example on how to use a parameter to the registered function:
  button->attachClick(fClicked, "me");

  // Here is an example on how to use an inline function:
  button->attachDoubleClick([]() { Serial.println("DoubleClick"); });

  // setup your own source of input:
  pinMode(PIN_INPUT, INPUT_PULLUP);

} // setup()

void loop()
{
  // read your own source of input:
  bool isPressed = (digitalRead(PIN_INPUT) == LOW);

  // call tick frequently with current push-state of the input
  button->tick(isPressed);
} // loop()
