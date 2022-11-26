
#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "GamePadModule.h"
#define PI 3.14159


//Class Constructor
GamePadModule::GamePadModule() : ModuleParent(GAMEPAD_ID)
{
	value =0;
	value0=0;
	//isCallBackAssigned=false;
}

//Up ArrowChecker
bool GamePadModule::isUpPressed()
{
	if(mode == 0)
	 return !!(value & (1<<UP_BIT));
    else
	 return 0;
}
//Down Arrow Checker 
bool GamePadModule::isDownPressed()
{   if(mode == 0)
	 return !!(value & (1<<DOWN_BIT));
    else
	 return 0;
}
//Left Arrow Checker
bool GamePadModule::isLeftPressed()
{   if(mode == 0)	
	 return  !!(value & (1<<LEFT_BIT));
    else
	 return 0;
}
//Right Arrow Checker
bool GamePadModule::isRightPressed()
{   if(mode == 0)
	 return !!(value & (1<<RIGHT_BIT));
    else
	 return 0;
}
//Orange Button Checker
bool GamePadModule::isStartPressed()
{
	return !!(value0 & (1<<START_BIT));
}
//Red Button Checker 
bool GamePadModule::isSelectPressed()
{
	
	return !!(value0 & (1<<SELECT_BIT));
}
//Green Button Checker 
bool GamePadModule::isTrianglePressed()
{
	return !!(value0 & (1<<TRIANGLE_BIT));
}
//Blue Button Checker
bool GamePadModule::isCirclePressed()
{
	
	return !!(value0 & (1<<CIRCLE_BIT));
}

bool GamePadModule::isCrossPressed()
{
	return !!(value0 & (1<<CROSS_BIT));
}

bool GamePadModule::isSquarePressed()
{
	return !!(value0 & (1<<SQUARE_BIT));
}

bool GamePadModule::isPressed(uint8_t a)   
{
if(a == 0)
{
	if(mode == 0)
	 return !!(value & (1<<UP_BIT));
    else
	return 0;
}	
else if(a==1)
{
	 if(mode == 0)
	 return !!(value & (1<<DOWN_BIT));
     else
	 return 0;
	
}
else if(a==2)
{
     if(mode == 0)	
	 return  !!(value & (1<<LEFT_BIT));
     else
	 return 0;	
}
else if(a==3)
{
	if(mode == 0)
	 return !!(value & (1<<RIGHT_BIT));
    else
	 return 0;
	
}
else if(a==4)
{
	return !!(value0 & (1<<START_BIT));
}
else if(a==5)
{
	return !!(value0 & (1<<SELECT_BIT));
}
else if(a==6)
{
	return !!(value0 & (1<<TRIANGLE_BIT));
}
else if(a==7)
{
	return !!(value0 & (1<<CIRCLE_BIT));
}
else if(a==8)
{
	return !!(value0 & (1<<CROSS_BIT));
}
else if(a==9)
{
	return !!(value0 & (1<<SQUARE_BIT));
}

}

//GamePad Input Data Processing  
void GamePadModule::processData()
{
	#ifdef DEBUG
	Serial.println("GamePad:processData");
	#endif
	//Checking Function-ID
	byte functionId =getDabbleInstance().getFunctionId();
	if(functionId==GAMEPAD_DIGITAL)
	{
		mode=0;
		value0=getDabbleInstance().getArgumentData(0)[0]; // 
		#ifdef DEBUG
		Serial.println(value0,BIN);
		#endif
		value=getDabbleInstance().getArgumentData(0)[1];
		#ifdef DEBUG
		Serial.println(value,BIN);
		#endif
		//Users Function Invoked
		/*if(isCallBackAssigned && !isInACallback())
		{
			enteringACallback();
			(*buttonChangeCallBack)(!!(value & (1<<UP_BIT)),      !!(value & (1<<DOWN_BIT)),
									!!(value & (1<<LEFT_BIT)), 	  !!(value & (1<<RIGHT_BIT)),
									!!(value0 & (1<<START_BIT)),  !!(value0 & (1<<SELECT_BIT)), 
									!!(value0 & (1<<TRIANGLE_BIT)),   !!(value0 & (1<<CIRCLE_BIT)));
			exitingACallback();
		}*/
	}
	else if(functionId == GAMEPAD_ANALOG)
	{
		mode=1;
		value0=getDabbleInstance().getArgumentData(0)[0]; // 
		#ifdef DEBUG
		Serial.println(value0,BIN);
		#endif
		value=getDabbleInstance().getArgumentData(0)[1];
		#ifdef DEBUG
		Serial.println(value,BIN);
		#endif
	}
	
	else if(functionId == GAMEPAD_ACCL)
	{
		mode=1;
		value0=getDabbleInstance().getArgumentData(0)[0]; // 
		#ifdef DEBUG
		Serial.println(value0,BIN);
		#endif
		value=getDabbleInstance().getArgumentData(0)[1];
		#ifdef DEBUG
		Serial.println(value,BIN);
		#endif
	}
	
}

float GamePadModule::getJoystickData(uint8_t b)
{
		uint16_t angle=((value >> 3)*15);
		uint8_t radius=value&0x07;
		float x_value= float(radius*(float(cos(float(angle*PI/180)))));
		float y_value= float(radius*(float(sin(float(angle*PI/180)))));
	if(b == 0)
	{
		
		return angle;
	}
	else if(b==1)
	{
		
		return radius;
	}
	else if(b==2)
	{
		return x_value;
	}
	else if(b==3)
	{
		return y_value;
	}
}


uint16_t GamePadModule::getAngle()
{
	if(mode == 1)
	 return ((value >> 3)*15);
    else
     return 0;		
}

uint8_t GamePadModule::getRadius()
{
	if(mode == 1)
	 return (value&0x07);
    else
     return 0;
}

float GamePadModule::getXaxisData()
{
	uint16_t angle=((value >> 3)*15);
	uint8_t radius=value&0x07;
	float x_value= float(radius*(float(cos(float(angle*PI/180)))));
	return x_value;
}

float GamePadModule::getYaxisData()
{
	uint16_t angle=((value >> 3)*15);
	uint8_t radius=value&0x07;
	float y_value= float(radius*(float(sin(float(angle*PI/180)))));
	return y_value;
}

float GamePadModule::getx_axis()
{
	uint16_t angle=((value >> 3)*15);
	uint8_t radius=value&0x07;
	float x_value= float(radius*(float(cos(float(angle*PI/180)))));
	return x_value;
}

float GamePadModule::gety_axis()
{
	uint16_t angle=((value >> 3)*15);
	uint8_t radius=value&0x07;
	float y_value= float(radius*(float(sin(float(angle*PI/180)))));
	return y_value;
}