#ifndef esp32PWMUtilities
#define esp32PWMUtilities
#include "Arduino.h"
void analogWrite(uint8_t pin, uint8_t pwm);
class TMotor{
public:
    TMotor();
    void attach(uint8_t , uint8_t ,uint8_t);
    void moveMotor(int pwm);
    void lockMotor();
    void freeMotor();
	void moveMotorS(uint8_t dir,uint8_t pwm); 
    
private:    
    uint8_t motorChannel;
    uint8_t pwmPin;
    uint8_t direction1;
    uint8_t direction2;
};

class Servo{
public:
    void write(uint8_t);
    void attach(uint8_t);
private:
    uint16_t minimumPulseWidth=544;
    uint16_t maximumPulseWidth=2400;
    uint8_t minimumAngle=0;
    uint8_t maximumAngle=180;
    uint8_t servoChannel;
    uint16_t degreeToMicroseconds(int);
    uint16_t microsecondsToPulsewidth(int);
};

#endif
