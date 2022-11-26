/*
   DabbleInputs module of your smartphone consists of two potentiometers, two slideswitches and two push button.

   You can reduce the size of library compiled by enabling only those modules that you want to
   use. For this first define CUSTOM_SETTINGS followed by defining INCLUDE_modulename.

   Explore more on: https://thestempedia.com/docs/dabble/input-module/
*/
#define CUSTOM_SETTINGS
#define INCLUDE_DABBLEINPUTS_MODULE
#include <DabbleESP32.h>
void setup() {
  Serial.begin(115200);     // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin("MyEsp32");  //set bluetooth name of your device
}

void loop() {
  Dabble.processInput();             //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.

  Serial.print("Pot1:");
  Serial.print(Inputs.getPot1Value());
  Serial.print('\t');
  Serial.print("Pot2:");
  Serial.print(Inputs.getPot2Value());
  Serial.print('\t');
  Serial.print("SS1:");
  Serial.print(Inputs.getSlideSwitch1Value());
  Serial.print('\t');
  Serial.print("SS2:");
  Serial.print(Inputs.getSlideSwitch2Value());
  Serial.print('\t');
  Serial.print("TS1:");
  Serial.print(Inputs.getTactileSwitch1Value());
  Serial.print('\t');
  Serial.print("TS2:");
  Serial.print(Inputs.getTactileSwitch2Value());
  Serial.println();
}
