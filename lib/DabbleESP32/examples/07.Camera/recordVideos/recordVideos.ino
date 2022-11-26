/*
  Camera Module allows you to click images and videos from smartphone by sending commands from your board.
  This function demonstrates functions available in library for camera module. 

  Open Serial monitor and follow the instructions printed there to take videos in different settings of camera.

   You can reduce the size of library compiled by enabling only those modules that you want
   to use. For this first define CUSTOM_SETTINGS followed by defining INCLUDE_modulename.

   Explore more on: https://thestempedia.com/docs/dabble/camera-module-photos-videos/
*/
#define CUSTOM_SETTINGS
#define INCLUDE_CAMERA_MODULE
#include <DabbleESP32.h>

void setup() {
  Serial.begin(115200);       // make sure your Serial Monitor is also set at this baud rate.
  Dabble.begin("MyEsp32");    //set bluetooth name of your device   
  printMessage();
}

void loop() {
  //upload code and open serial monitor (reset your board if you cannot se any message printed on Serial Monitor)
  Dabble.processInput();  //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  char a = processSerialdata();
  if( a == '1')
  {
    Camera.setParameters(FRONT,OFF,HIGH_QUALITY,0);  //Direction , Flash, Quality, zoom(0-100%)
    Camera.startRecording();
  }
  if( a == '2')
  {
    Camera.flipTo(REAR);
    Camera.flashMode(AUTO);
    Camera.setQuality(LOW_QUALITY);
    Camera.startRecording();
  }
  if(a == '3')
  {
    Camera.flipTo(REAR);
    Camera.setQuality(HIGH_QUALITY);
    Camera.zoom(50);
    Camera.startRecording();
  }
  
  if(a == '4')
  {
    Camera.stopRecording();
  }
 

}

void printMessage()
{
  Serial.println("Enter any number between 1 to 4 for executing task corresponding to that number: ");
  Serial.println("Tasks executed on sending different numbers are as followed: ");
  Serial.println("1 - Take a high quality video from front camera with no flash and no zoom.");
  Serial.println("2 - Take a low quality video from rear camera with Auto flash");
  Serial.println("3 - Take a 50% zoomed image from Rear camera with high quality"); 
  Serial.println("4 - Stop video recording");
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
