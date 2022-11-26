 /*
   If there is temperature sensor in your smartphone then you can access it through this example.

   You can reduce the size of library compiled by enabling only those modules that you want
   to   use. For this first define CUSTOM_SETTINGS followed by defining INCLUDE_modulename.

   Explore more on: https://thestempedia.com/docs/dabble/phone-sensors-module/
*/
#define CUSTOM_SETTINGS
#define INCLUDE_SENSOR_MODULE
#include <DabbleESP32.h>


void setup() {
  Serial.begin(115200);     // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin("MyEsp32");     //Enter baudrate of your bluetooth.Connect bluetooth on Bluetooth port present on evive.
}

void loop() {
  Dabble.processInput();             //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  print_Temperature_data();
}

void print_Temperature_data()
{
  Serial.print("TEMPERATURE: ");
  Serial.println(Sensor.getTemperature(), 7);
  Serial.println();
}
