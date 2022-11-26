/*
   MotorControl Module is used to control actuators like DC motors and Servos.
   
   NOTE: If you are interested in using any other pin as PWM pin then use led channels 
   from channel 4 onwards on ESP32.Because first four channels are for controlling motor and servo from
   Motor control module of Dabble.
   
   
   You can reduce the size of library compiled by enabling only those modules that you want to
   use. For this first define CUSTOM_SETTINGS followed by defining INCLUDE_modulename.

   Explore more on: https://thestempedia.com/docs/dabble/motor-control-module/

*/
#define CUSTOM_SETTINGS
#define INCLUDE_MOTORCONTROL_MODULE
#include <DabbleESP32.h>


uint8_t pinServo1 =  4;
uint8_t pinServo2 =  5;
uint8_t pwmMotor1 =  12;
uint8_t dir1Motor1 = 13;
uint8_t dir2Motor1 = 14;
uint8_t pwmMotor2 =  21;
uint8_t dir1Motor2 = 22;
uint8_t dir2Motor2 = 23;

void setup() {
  Serial.begin(115200);         // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin("MyEsp32");      //set bluetooth name of your device  
}

void loop() {
  Dabble.processInput();             //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.              //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  Controls.runServo1(pinServo1);
  Controls.runServo2(pinServo2);
  Controls.runMotor1(pwmMotor1,dir1Motor1,dir2Motor1);             
  Controls.runMotor2(pwmMotor2,dir1Motor2,dir2Motor2);             
}
