#ifndef GamePadModule_h
#define GamePadModule_h

#include "ModuleParent.h"

//Input Function ID
#define GAMEPAD_DIGITAL 0x01
#define GAMEPAD_ANALOG 0x02
#define GAMEPAD_ACCL   0x03
//GamePad Bit Reference
//Byte 1
#define START_BIT 0
#define SELECT_BIT 1
#define TRIANGLE_BIT 2 
#define CIRCLE_BIT 3
#define CROSS_BIT 4
#define SQUARE_BIT 5

//Byte 2 in case of Digital Mode GamePad
#define UP_BIT 0
#define DOWN_BIT 1
#define LEFT_BIT 2
#define RIGHT_BIT 3



//Byte 2 in case of Analog/Accelerometer Mode GamePad
//XXXXXYYY = XXXXX(*15) is angle in radians, YYY is radius

class GamePadModule : public ModuleParent
{
public:
	//Constructor
	GamePadModule();
	//Arduino
	//Checker Functions
	bool isStartPressed();
	bool isSelectPressed();
	bool isTrianglePressed();
	bool isCirclePressed();
	bool isCrossPressed();
	bool isSquarePressed();
	bool isUpPressed();
	bool isDownPressed();
	bool isLeftPressed();
	bool isRightPressed();
	uint16_t getAngle();
	uint8_t getRadius();
	float getXaxisData();
	float getYaxisData();

	float getx_axis();	
    float gety_axis();
	
	//Pictoblox
	bool isPressed(uint8_t a);
	float getJoystickData(uint8_t b);
	
	//setOnChange for Users Function
	/* void setOnButtonChange(void (*)(unsigned char , unsigned char ,
									unsigned char , unsigned char ,
									unsigned char , unsigned char ,
									unsigned char , unsigned char ));
 */private:
	//Reserve Variables
	bool mode;   //mode=0 for gamepad , mode = 1 for joystick
	byte value;
	byte value0;
	//bool isCallBackAssigned;
	//Process Input Data  
	void processData();
	/*void (*buttonChangeCallBack)(unsigned char , unsigned char ,
								 unsigned char , unsigned char ,
								 unsigned char ,unsigned char  ,
								 unsigned char ,unsigned char );*/
};

//Extern Object
extern GamePadModule GamePad;
#endif 