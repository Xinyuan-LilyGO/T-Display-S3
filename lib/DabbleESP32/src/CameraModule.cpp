#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "CameraModule.h"

CameraModule::CameraModule(): ModuleParent(CAMERA_ID)
{
	 
}
void CameraModule::setParameters(uint8_t rotateTo,uint8_t flashmode, uint8_t quality,uint8_t zoomvalue)
{
	
	Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&rotateTo));
	Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&flashmode));
	Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&quality));
	Dabble.sendModuleFrame(CAMERA_ID,0,0x02,1,new FunctionArg(1,&zoomvalue));
}
void CameraModule::captureImage()
{
   Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&captureimage));
}

void CameraModule::startRecording()
{
	Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&startVideo));
}

void CameraModule::stopRecording()
{
	Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&stopVideo));
}

void CameraModule::flashMode(uint8_t a)
{
	 Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&a));
}

void CameraModule::setQuality(uint8_t a)
{
	 Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&a));
}

void CameraModule::zoom(uint8_t a)
{
	 Dabble.sendModuleFrame(CAMERA_ID,0,0x02,1,new FunctionArg(1,&a));
}

void CameraModule::flipTo(uint8_t direction)
{
	if(direction == 1)  //Rear
	{
	 direction = 0x05;
	 Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&direction));	
	}
	else if(direction == 2)  //Front
	{
	 direction = 0x04;
	 Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&direction));
	}
	else
		Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&direction));
}

void CameraModule::cameraAction(uint8_t a)
{
	if(a == 1)
	{
	  Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&captureimage));
	}
	if(a == 2)
	{
	  Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&startVideo));
	}
	if(a == 3)
	{
	  Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&stopVideo));
	}
}

void CameraModule::cameraConfig(uint8_t flash,uint8_t quality,uint8_t zoom)
{
	if(flash == 1)        //On
	{
		flash =	0x06;
		Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&flash));
	}
	else if(flash == 2)        //Auto
	{
		flash =	0x07;
		Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&flash));
	}
	else if(flash == 3)        //Off
	{
		flash =	0x08;
		Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&flash));
	}
	
	if(quality == 1)
	{
		quality = 0x09;        //High
		Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&quality));
	}
	else if(quality == 2)
	{
		quality = 0x0A;     //Low
		Dabble.sendModuleFrame(CAMERA_ID,0,0x01,1,new FunctionArg(1,&quality));
	}
	Dabble.sendModuleFrame(CAMERA_ID,0,0x02,1,new FunctionArg(1,&zoom));
}
