/*
  Notification module can be used for notifying your mobile about status of various activities happening on your hardware.
  In this example a push button is connected to a digital pin. And mobile is notified about how many times that push button
  is pressed.

  You can reduce the size of library compiled by enabling only those modules that you want to
  use. For this first define CUSTOM_SETTINGS followed by defining INCLUDE_modulename.

  Explore more on: https://thestempedia.com/docs/dabble/
*/

#define CUSTOM_SETTINGS
#define INCLUDE_NOTIFICATION_MODULE
#include <DabbleESP32.h>
uint8_t pushButtonPin = 2;
int counts = 0;

void setup() {
  Serial.begin(115200);
  Dabble.begin("MyEsp32");                     //set bluetooth name of your device
  pinMode(pushButtonPin, INPUT_PULLUP);        //Since pin is internally pulled up hence connect one side of push button to ground so whenever button is pushed pin reads LOW logic.
  Dabble.waitForAppConnection();               //waiting for App to connect
  Notification.clear();                        //clear previous notifictions
  Notification.setTitle("Button Counts");      //Enter title of your notification
}

void loop() {
  Dabble.processInput();    //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  if (digitalRead(pushButtonPin) == LOW)
  {
    counts++;
    delay(1000);   //debounce delay
  }
  Notification.notifyPhone(String("Button has been pressed ") + counts + String (" time"));
}
