/*
   Data Logger module helps you in storing data in form of .csv file. 
   Later you can open this file to view your stored data.
   
   You can reduce the size of library compiled by enabling only those modules that you want to
   use.For this first define CUSTOM_SETTINGS followed by defining INCLUDE_modulename.
   
   Explore more on: https://thestempedia.com/docs/dabble/
*/
#define CUSTOM_SETTINGS
#define INCLUDE_SENSOR_MODULE
#define INCLUDE_DATALOGGER_MODULE
#include <DabbleESP32.h>
bool isFileOpen = true;
uint8_t closeFileSignalPin = 2;   //this pin is internally pulled up and a push button grounded on one side is connected to pin so that pin detects low logic when push button is pressed.

void initializeFile(){
  Serial.println("Initialize");
  DataLogger.createFile("Microphone");
  DataLogger.createColumn("Decibel");
}

void setup() { 
  pinMode(closeFileSignalPin,INPUT_PULLUP);
  Serial.begin(115200);       // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin("Myesp32");    //set bluetooth name of your device
  DataLogger.sendSettings(&initializeFile);
}

void loop() {
  Dabble.processInput();             //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  if( isFileOpen == true)
  {
    print_Sound_data();
    DataLogger.send("Decibel",Sensor.getdata_Sound());
  }
  if((digitalRead(closeFileSignalPin) == LOW) && isFileOpen == true)
  {
    isFileOpen = false;
    DataLogger.stop();
  }
}

void print_Sound_data()
{
  Serial.print("SOUND: ");
  Serial.println(Sensor.getdata_Sound(), 3);
  Serial.println();
}