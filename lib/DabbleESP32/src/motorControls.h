#ifndef motorControls_h
#define motorControls_h

#include "ModuleParent.h"




class motorControls : public ModuleParent
{
	
	public:
	
    motorControls();
	//Pictobox and Arduino
	void runMotor1(uint8_t pwm,uint8_t direction1,uint8_t direction2);
    void runMotor2(uint8_t pwm,uint8_t direction1,uint8_t direction2);
	void runServo1(uint8_t pin);
	void runServo2(uint8_t pin);
	
	public:
	 byte byte1;
     byte byte2;
	uint8_t angleServo1=0;
    uint8_t angleServo2=0;
	int pwmMotor1=0;
	int pwmMotor2=0;
    byte functionId;
	


    private:	
	void processData();
	uint8_t prevMotor1pin = 0;
	uint8_t prevMotor2pin = 0;
	uint8_t prevServo1pin = 0;
	uint8_t prevServo2pin = 0;
	static int minPulseWidth;
	static int maxPulseWidth;
	static int minAngle;
	static int maxAngle;
	int angleTomicroseconds(int degree);
	int microsecondsToDuty(int pulse);
	void writeServoAngle(int angle,uint8_t channel);
};


//void runMotor1(int a);
//void runMotor2(int b);


extern motorControls Controls;

#endif 