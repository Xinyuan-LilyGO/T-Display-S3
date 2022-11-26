/*
   With this block of phone sensor module you can access gyroscope values of your smartphone.
   Gyroscope gives you angular acceleration in x,y and z axis.

   You can reduce the size of library compiled by enabling only those modules that you want
   to   use. For this first define CUSTOM_SETTINGS followed by defining
   INCLUDE_modulename.

   Explore more on: https://thestempedia.com/docs/dabble/phone-sensors-module/
*/
#define CUSTOM_SETTINGS
#define INCLUDE_SENSOR_MODULE
#include <DabbleESP32.h>


void setup() {
  Serial.begin(115200);   // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin("MyEsp32"); //set bluetooth name of your device
}

void loop() {
  Dabble.processInput();             //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  print_Gyroscope_data();
}

void print_Gyroscope_data()
{
  Serial.print("X-axis: ");
  Serial.print(Sensor.getGyroscopeXaxis());
  Serial.print('\t');
  Serial.print("Y-axis: ");
  Serial.print(Sensor.getGyroscopeYaxis());
  Serial.print('\t');
  Serial.print("Z-axis: ");
  Serial.println(Sensor.getGyroscopeZaxis());
  Serial.println();
}
