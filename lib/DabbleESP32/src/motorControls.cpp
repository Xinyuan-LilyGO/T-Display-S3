#define FROM_DABBLE_LIBRARY
#include "DabbleESP32.h"
#include "motorControls.h"

int motorControls::minPulseWidth = 544;
int motorControls::maxPulseWidth = 2400;
int motorControls::minAngle = 0;
int motorControls::maxAngle = 180;

	
motorControls::motorControls() : ModuleParent(CONTROLS_ID)
{

}

void motorControls::processData()
{
	   /* #ifdef DEBUG
	    Serial.println("Controls:processData");
		#endif */
		functionId=getDabbleInstance().getFunctionId();
		if(functionId == 0x01 || functionId == 0x02)
		{
			
		byte1=getDabbleInstance().getArgumentData(0)[0]; 
		byte2=getDabbleInstance().getArgumentData(0)[1];
		#ifdef DEBUG
		Serial.print("byte1: ");
		Serial.print(byte1);
		Serial.print("byte2: ");
	    Serial.println(byte2);
		#endif
		if(functionId == 0x01)
		{
			
			if(byte1 == 0xf0)
			{
				pwmMotor1=byte2;
			}
			else if(byte1 == 0xff)
			{
				pwmMotor1= -byte2;
			}
			else if(byte1 == 0x0f)
			{
				pwmMotor1= 0;
			}
			else if(byte1 == 0x00)
			{
				pwmMotor1= 0;
			}
			
		}
		
		if(functionId == 0x02)
		{
			if(byte1 == 0xf0)
			{
				pwmMotor2= byte2;
			}
			else if(byte1 == 0xff)
			{
				pwmMotor2= -byte2;
			}
			else if(byte1 == 0x0f)
			{
				pwmMotor2= 0;
			}
			else if(byte1 == 0x00)
			{
				pwmMotor2= 0;
			}
		}
		}
		else if(functionId == 0x03 || functionId == 0x04)
		{
			byte1=getDabbleInstance().getArgumentData(0)[0]; 
			#ifdef DEBUG
			Serial.print("byte1:");
			Serial.println(byte1);
			#endif
			if(functionId == 0x03)
			{
				angleServo1=byte1;
			}
			else if(functionId == 0x04)
			{
			    angleServo2=byte1;
			}
		}
		
}

void motorControls::runMotor1(uint8_t pwm,uint8_t direction1,uint8_t direction2)
{
	pinMode(direction1,OUTPUT);
	pinMode(direction2,OUTPUT);
	if(prevMotor1pin != pwm)
	{
		ledcAttachPin(pwm,0);
		ledcSetup(0,100,8);
	}
	if(pwmMotor1 > 0)
	{
		digitalWrite(direction1,HIGH);
		digitalWrite(direction2,LOW);
		ledcWrite(0,pwmMotor1);
	}
	else if(pwmMotor1 < 0)
	{
		digitalWrite(direction1,LOW);
		digitalWrite(direction2,HIGH);
		ledcWrite(0,-pwmMotor1);  //making negative pwm value positive
	}
	else
	{
		digitalWrite(direction1,LOW);
		digitalWrite(direction2,LOW);
		ledcWrite(0,0);
	}
	prevMotor1pin = pwm;
}

void motorControls::runMotor2(uint8_t pwm,uint8_t direction1,uint8_t direction2)
{
	pinMode(direction1,OUTPUT);
	pinMode(direction2,OUTPUT);
	if(prevMotor2pin != pwm)
	{
		ledcAttachPin(pwm,1);
		ledcSetup(1,100,8);
	}
	if(pwmMotor2 > 0)
	{

		/*#ifdef DEBUG
		Serial.print(direction1);
		Serial.print(" ");
		Serial.println(direction2);
		Serial.println("Clockwise");
		#endif*/
		digitalWrite(direction1,HIGH);
		digitalWrite(direction2,LOW);
		ledcWrite(1,pwmMotor2);
	}
	else if(pwmMotor2 < 0)
	{
		/*#ifdef DEBUG
		Serial.print(direction1);
		Serial.print(" ");
		Serial.println(direction2);
		Serial.println("Anti-Clockwise");*
		#endif*/
		digitalWrite(direction1,LOW);
		digitalWrite(direction2,HIGH);
		ledcWrite(1,-pwmMotor2);  //making negative pwm value positive
	}
	else
	{
		digitalWrite(direction1,LOW);
		digitalWrite(direction2,LOW);
		ledcWrite(1,0);
	}
	prevMotor2pin = pwm;
}

void motorControls::runServo1(uint8_t pin)    //Attach Servo1 to channel 3
{
	if(prevServo1pin!=pin)
	{
		ledcAttachPin(pin,3);
		ledcSetup(3,50,16);
	}
	writeServoAngle(angleServo1,3);
	prevServo1pin = pin;
}

void motorControls::runServo2(uint8_t pin)	 //Attach Servo2 to channel 4
{
    if(prevServo2pin!=pin)
	{
		ledcAttachPin(pin,4);
		ledcSetup(4,50,16);
	}
	writeServoAngle(angleServo2,4);
	prevServo2pin = pin;
}

int motorControls::angleTomicroseconds(int degree)
{
	degree = constrain(degree,minAngle,maxAngle);
	return map(degree,minAngle,maxAngle,minPulseWidth,maxPulseWidth);
}

int motorControls::microsecondsToDuty(int pulse)
{
	pulse=constrain(pulse,minPulseWidth,maxPulseWidth);
	return map(pulse,0,20000,0,65535);
}

void motorControls::writeServoAngle(int angle,uint8_t channel)
{
	/*Serial.print(angle);
	Serial.print(" ");
	Serial.println(channel);*/
	int _pulse = angleTomicroseconds(angle);
	int _duty =  microsecondsToDuty(_pulse);
	ledcWrite(channel,_duty);
}