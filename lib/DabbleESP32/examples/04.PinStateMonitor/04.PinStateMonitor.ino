/*
   Pin State Monitor is made for monitoring status of analog and digital pins of your board.
   In this example bluetooth is to be connected on HardwareSerial0 for Uno and Nano Boards.

   NOTE: State for only  following pins can be seen on Pin State Monitor:
   | Screen Name  | GPIO Pins                          |
   | Digital      | 2,4,5,12,13,14,15,16,17,18,19,21   |
   |              | 23,25,26,27                        |
   | Analog       | 32,33,34,35,36,39                  |
   
   You can reduce the size of library compiled by enabling only those modules that you want
   to use. For this first define CUSTOM_SETTINGS followed by defining INCLUDE_modulename.

   Explore more on: https://thestempedia.com/docs/dabble/pin-state-monitor-module/
*/

#define CUSTOM_SETTINGS
#define INCLUDE_PINMONITOR_MODULE
#include <DabbleESP32.h>


void setup() {
  /*
     NOTE: PinMonitor only displays status of the pins of your board. It does not configure pinMode of the pins.
     So if there is any necessity to define pinMode then declare it setup as per requirement.
  */
  Serial.begin(115200);    // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin("MyEsp32");    //set bluetooth name of your device
}

void loop() {
  Dabble.processInput();             //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  PinMonitor.sendDigitalData();
  PinMonitor.sendAnalogData();
  delayMicroseconds(20);
}
