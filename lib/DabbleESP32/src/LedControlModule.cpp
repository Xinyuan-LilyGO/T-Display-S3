#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "LedControlModule.h"

#ifdef ESP32
#define MAX_PWM_CHANNELS	8
uint8_t freeChannel =0,prevPin=0,currentChannel=0;
bool pinType=0;   // flag to differentiate pin behaviour means whether pin will be traeted as PWM or digital
#endif

LedControlModule::LedControlModule(): ModuleParent(LEDCONTROL_ID)
{
}

void LedControlModule::processData()
{
	byte   functionId =getDabbleInstance().getFunctionId();
	  
	if(functionId == 0x01)
	{
		pin = getDabbleInstance().getArgumentData(0)[0]; 	
		if(prevPin != pin)
		{
			if(freeChannel < MAX_PWM_CHANNELS)
			{
				#ifdef DEBUG
				Serial.println("Pin is treated as pwm pin");
				#endif
				/*#ifdef DEBUG
				Serial.print("Channels: ");
				Serial.print(freeChannel);
				Serial.print(" ");
				Serial.println(currentChannel);
				#endif*/
				currentChannel = freeChannel;
				ledcAttachPin(pin,currentChannel);
				ledcSetup(currentChannel,100,8);
				freeChannel++;
				/*#ifdef DEBUG
				Serial.print("freeChannels: ");
				Serial.println(freeChannel);
				#endif*/
				pinType =1;     // pin is a PWM type
			}
			else
			{
				#ifdef DEBUG
				Serial.println("Pin is treated as digital pin");
				#endif
				pinMode(pin,OUTPUT);   
				pinType = 0;      // pin is a digital type
			}
		}
		prevPin = pin;
	}
	else if (functionId == 0x03)    //ON as per Brightness value
	{
	    brightness = getDabbleInstance().getArgumentData(0)[0]; 	 
	    #ifdef DEBUG   
			Serial.println(brightness);
	    #endif
	    value  = map(brightness,0,100,0,255);
	    #ifdef DEBUG
			Serial.println(value);
	    #endif
	}
	else if(functionId == 0x02)         //OFF
	{
	   brightness =0;
	   value=0;
	}
	
	if(pinType == 1 && pin!= 0)      //for PWM type pin
	{
		//Serial.println("Value Assigned");
		ledcWrite(currentChannel,value);
	}
	else                             //for digital type pin
	{
		//Serial.println("Value Assigned");
		if(value == 255)
		{
			digitalWrite(pin,HIGH);
		}
		else
		{
			digitalWrite(pin,LOW);
		}
	}
}

uint8_t LedControlModule::readBrightness()
{
return brightness;
}

bool LedControlModule::getpinState()
{
 if(brightness>0)
	return 1; 
 else
	return 0;
}

uint8_t  LedControlModule::getpinNumber()
{
 return pin;
}

