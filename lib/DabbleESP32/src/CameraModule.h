#ifndef CameraModule_h
#define CameraModule_h
#include "ModuleParent.h"

#define REAR   0x05
#define FRONT  0x04

#define ON     0x06
#define OFF    0x08
#define AUTO   0x07

#define HIGH_QUALITY 0x09 
#define LOW_QUALITY  0x0A
 


class CameraModule:public ModuleParent
{
public:
CameraModule();

//Arduino
void setParameters(uint8_t,uint8_t,uint8_t,uint8_t);  //Direction,flash,quality,zoom
void captureImage();      //Camera Direction
void startRecording();    
void stopRecording();
void flashMode(byte);      //On, Off, Auto
void setQuality(uint8_t);   //High, Low
void zoom(uint8_t);         // enter zoom in % from 1 to 100.

//Arduino and Pictoblox
void flipTo(uint8_t);      //1 Front 2 Rear

//Pictoblox
void cameraAction(uint8_t); //1 captureImage,2 startVideo,3 stopVideo
void cameraConfig(uint8_t,uint8_t,uint8_t);//flash,quality,zoom
private:
uint8_t captureimage = 0x01;
uint8_t startVideo = 0x02;
uint8_t stopVideo  = 0x03;
};
extern CameraModule Camera;
#endif
