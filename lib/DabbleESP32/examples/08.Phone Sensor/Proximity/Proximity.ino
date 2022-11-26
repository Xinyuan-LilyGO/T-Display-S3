/*
   Proximity block allows to access proximity sensor of your mobile. This sensor in mobile
   phone gives two different values depending on the fact is object is near or far. You will get
   0 reading when object is very close to phone and any random number if object is far.

   You can reduce the size of library compiled by enabling only those modules that you want
   to   use. For this first define CUSTOM_SETTINGS followed by defining INCLUDE_modulename.

   Explore more on: https://thestempedia.com/docs/dabble/phone-sensors-module/
*/
#define CUSTOM_SETTINGS
#define INCLUDE_SENSOR_MODULE
#include <DabbleESP32.h>


void setup() {
  Serial.begin(115200);  // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin("MyEsp32");  ////set bluetooth name of your device
}

void loop() {
  Dabble.processInput();             //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  print_Proximity_data();
}

void print_Proximity_data()
{
  Serial.print("Distance: ");
  Serial.println(Sensor.getProximityDistance(), 7);
  Serial.println();
}
