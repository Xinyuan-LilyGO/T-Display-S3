/*
 This is a sample sketch to show how to use the OneButtonLibrary
 to detect click events on 2 buttons in parallel. 
 The library internals are explained at
 http://www.mathertel.de/Arduino/OneButtonLibrary.aspx
 
 Setup a test circuit:
 * Connect a pushbutton to pin A1 (ButtonPin) and ground.
 * Connect a pushbutton to pin A2 (ButtonPin) and ground.
 * The Serial interface is used for output the detected button events.
 
 The Sketch shows how to setup the library and bind 2 buttons to their functions.
 In the loop function the button1.tick and button2.tick functions have to be called as often as you like.
 */

// 01.03.2014 created by Matthias Hertel
// ... and working.

/* Sample output:

Starting TwoButtons...
Button 1 click.
Button 2 click.
Button 1 doubleclick.
Button 2 doubleclick.
Button 1 longPress start
Button 1 longPress...
Button 1 longPress...
Button 1 longPress...
Button 1 longPress stop
Button 2 longPress start
Button 2 longPress...
Button 2 longPress...
Button 2 longPress stop

*/

#include "OneButton.h"

// Setup a new OneButton on pin A1.  
OneButton button1(A1, true);
// Setup a new OneButton on pin A2.  
OneButton button2(A2, true);


// setup code here, to run once:
void setup() {
  // Setup the Serial port. see http://arduino.cc/en/Serial/IfSerial
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("Starting TwoButtons...");

  // link the button 1 functions.
  button1.attachClick(click1);
  button1.attachDoubleClick(doubleclick1);
  button1.attachLongPressStart(longPressStart1);
  button1.attachLongPressStop(longPressStop1);
  button1.attachDuringLongPress(longPress1);

  // link the button 2 functions.
  button2.attachClick(click2);
  button2.attachDoubleClick(doubleclick2);
  button2.attachLongPressStart(longPressStart2);
  button2.attachLongPressStop(longPressStop2);
  button2.attachDuringLongPress(longPress2);

} // setup


// main code here, to run repeatedly: 
void loop() {
  // keep watching the push buttons:
  button1.tick();
  button2.tick();

  // You can implement other code in here or just wait a while 
  delay(10);
} // loop


// ----- button 1 callback functions

// This function will be called when the button1 was pressed 1 time (and no 2. button press followed).
void click1() {
  Serial.println("Button 1 click.");
} // click1


// This function will be called when the button1 was pressed 2 times in a short timeframe.
void doubleclick1() {
  Serial.println("Button 1 doubleclick.");
} // doubleclick1


// This function will be called once, when the button1 is pressed for a long time.
void longPressStart1() {
  Serial.println("Button 1 longPress start");
} // longPressStart1


// This function will be called often, while the button1 is pressed for a long time.
void longPress1() {
  Serial.println("Button 1 longPress...");
} // longPress1


// This function will be called once, when the button1 is released after beeing pressed for a long time.
void longPressStop1() {
  Serial.println("Button 1 longPress stop");
} // longPressStop1


// ... and the same for button 2:

void click2() {
  Serial.println("Button 2 click.");
} // click2


void doubleclick2() {
  Serial.println("Button 2 doubleclick.");
} // doubleclick2


void longPressStart2() {
  Serial.println("Button 2 longPress start");
} // longPressStart2


void longPress2() {
  Serial.println("Button 2 longPress...");
} // longPress2

void longPressStop2() {
  Serial.println("Button 2 longPress stop");
} // longPressStop2


// End

