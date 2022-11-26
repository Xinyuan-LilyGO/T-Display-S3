/*
   Camera Module allows you to click images and take videos from smartphone by sending commands from your hardware.
   This function demonstrates functions available in library for camera module. 

   Open Serial monitor and follow the instructions printed there to take images and videos in different cases.

   You can reduce the size of library compiled by enabling only those modules that you want
   to use. For this first define CUSTOM_SETTINGS followed by defining INCLUDE_modulename.

   Explore more on: https://thestempedia.com/docs/dabble/camera-module-photos-videos/
*/
#define CUSTOM_SETTINGS
#define INCLUDE_CAMERA_MODULE
#include <DabbleESP32.h>

void setup() {
  Serial.begin(115200);       // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin("MyEsp32");     //set bluetooth name of your device
  printMessage();
}

void loop() {
  //upload code and open serial monitor (reset your board once if you cannot see any message printed on Serial Monitor)
  Dabble.processInput();  //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  
  char a = processSerialdata();
  if( a == '1')
  {
    Camera.setParameters(FRONT,OFF,HIGH_QUALITY,0);   //Camera Direction, Flash, quality, Zoom(from 0 to 100%)
    Camera.captureImage();
  }
  if( a == '2')
  {
    Camera.flipTo(REAR);
    Camera.flashMode(OFF);
    Camera.setQuality(LOW_QUALITY);
    Camera.captureImage();
  }
  if(a == '3')
  {
    Camera.flipTo(REAR);
    Camera.setQuality(HIGH_QUALITY);
    Camera.zoom(50);
    Camera.captureImage();
  }
 

}

void printMessage()
{
  Serial.println("Enter any number between 1 to 3 for executing task corresponding to that number: ");
  Serial.println("Tasks executed on sending different numbers are as followed: ");
  Serial.println("1 - Take a high quality image from front camera with no flash and no zoom.");
  Serial.println("2 - Take a low quality image from rear camera with Off flash and no zoom");
  Serial.println("3 - Take a 50% zoomed image from Rear camera with high quality"); 
}

char processSerialdata()
{
  if(Serial.available()!=0)
  {
   return Serial.read(); 
  }
  else
  {
    return '0';
  }
}
