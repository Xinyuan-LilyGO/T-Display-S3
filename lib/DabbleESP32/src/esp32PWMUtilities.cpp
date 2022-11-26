#include "esp32PWMUtilities.h"

static bool freeChannelFlag[7] = {0,0,0,0,0,0,0};
static uint8_t currentFreeChannel = 6;
static int channelPinMap[7] = {-1,-1,-1,-1,-1,-1,-1};
int frequency=50;
uint8_t totalFreeChannel = 0;
static bool flag = false;
//PWM control

void analogWrite(uint8_t pin , uint8_t value)
{
    uint8_t channel;
    bool isNewPin = true;
    for(int8_t i = 0 ; i< 7; i++)
    {
        if(channelPinMap[i] == pin)
        {
            channel = i;
            isNewPin = false;
            //////Serial.println("case1");
            break;
        }
        if(i == (6) && isNewPin == true)
        {
            Serial.println("case2");
            if(currentFreeChannel >= totalFreeChannel)
            {
                Serial.println("case3");
                ledcAttachPin(pin , currentFreeChannel);
                channelPinMap[currentFreeChannel] = pin;
                ledcSetup(currentFreeChannel,frequency,8);
                channel = currentFreeChannel;
                currentFreeChannel--;
            }
        }
    }
    //Serial.print("channel: ");
    //Serial.println(channel);
    ledcWrite(channel,value);
}


//TMotor

TMotor::TMotor(){
}

void TMotor::attach(uint8_t motorpin, uint8_t dir1, uint8_t dir2)
{
    /************For Exanpder****************/
    //expander.pinMode(dir1,OUTPUT);
    //exapnder.pinMode(dir2,OUTPUT);
    /*******************************/
    
    pinMode(dir1,OUTPUT);
    pinMode(dir2,OUTPUT);
    
    if(flag == false)                 ///////////////////////////
    {
        flag = true;
        //Serial.println("Initialized");
        //expander.begin();
    }                                 ///////////////////////////
    //////Serial.println("pin: " + String(motorpin));
    bool isNewPin = true;
    pwmPin = motorpin;
    direction1 = dir1;
    direction2 = dir2;
    for(int8_t i = 0 ; i < 7; i++)
    {
        if(channelPinMap[i] == motorpin)
        {
            motorChannel = i;
            isNewPin = false;
            Serial.println("case4");
            break;
        }
        if(i == (6) && isNewPin == true)
        {
            Serial.println("case5");
            if(currentFreeChannel >= totalFreeChannel)
            {
                Serial.println("case6");
                ledcAttachPin(motorpin , currentFreeChannel);
                Serial.println("channel: " + String(currentFreeChannel) +"  " + String(motorpin));
                channelPinMap[currentFreeChannel] = motorpin;
                Serial.println(channelPinMap[currentFreeChannel]);
                ledcSetup(currentFreeChannel,frequency,8);
                motorChannel = currentFreeChannel; 
                currentFreeChannel--;
            }
        }
        Serial.println("channelPinMap: " + String(channelPinMap[i]));
    }
    Serial.println("motorChannel: " + String(motorChannel));
}

void TMotor::moveMotor(int pwm)
{
    Serial.println("motorChannelf: " + String(motorChannel));
    if(pwm >= 0)
    {
        //expander.digitalWrite(direction1,HIGH);
        //expander.digitalWrite(direction2,LOW);
        digitalWrite(direction1,HIGH);
        digitalWrite(direction2,LOW);
        ledcWrite(motorChannel,pwm);
    }
    if(pwm < 0)
    {
        //expander.digitalWrite(direction1,LOW);
        //expander.digitalWrite(direction2,HIGH);
        digitalWrite(direction1,LOW);
        digitalWrite(direction2,HIGH);
        ledcWrite(motorChannel,pwm);
    }
}

void TMotor::lockMotor()
{
   // expander.digitalWrite(direction1,HIGH);
   // expander.digitalWrite(direction2,HIGH); 
    digitalWrite(direction1,HIGH);
    digitalWrite(direction2,HIGH);
    ledcWrite(motorChannel,0);
}

void TMotor::freeMotor()
{
    digitalWrite(direction1,LOW);
    digitalWrite(direction2,LOW);
    ledcWrite(motorChannel,0);
}

void TMotor::moveMotorS(uint8_t dir,uint8_t pwm)
{
	if(dir == 1)
	{
		digitalWrite(direction1,HIGH);
        digitalWrite(direction2,LOW);
        ledcWrite(motorChannel,pwm);
	}
	if(dir == 2)
	{
		digitalWrite(direction1,LOW);
        digitalWrite(direction2,HIGH);
        ledcWrite(motorChannel,pwm);
	}
}
	

//Servo

void Servo::attach(uint8_t pin)
{
    bool isNewPin = true;
    for(int8_t i = 0 ; i<7; i++)
    {
        if(channelPinMap[i] == pin)
        {
            servoChannel = i;
            isNewPin = false;
            break;
        }
        if(i == (6) && isNewPin == true)
        {
            if(currentFreeChannel >= totalFreeChannel)
            {
                ledcAttachPin(pin , currentFreeChannel);
                channelPinMap[currentFreeChannel] = pin;
                ledcSetup(currentFreeChannel,50,16);
                servoChannel = currentFreeChannel; 
                currentFreeChannel--;
            }
        }
      Serial.println("channelPinMap: " + String(channelPinMap[i]));
    }
    Serial.println("servoChannel: " + String(servoChannel));
}

uint16_t Servo::degreeToMicroseconds(int degree)
{
    degree = constrain(degree,minimumAngle,maximumAngle);
  return map(degree,minimumAngle,maximumAngle,minimumPulseWidth,maximumPulseWidth);
}

uint16_t Servo::microsecondsToPulsewidth(int pulse)
{
    pulse=constrain(pulse,minimumPulseWidth,maximumPulseWidth);
  return map(pulse,0,20000,0,65535);
}

void Servo::write(uint8_t angle)
{
    Serial.println("servoChannelf: " + String(servoChannel));
	uint16_t pulse = degreeToMicroseconds(angle);
	Serial.print(pulse);
	Serial.print(" ");
	uint16_t duty = microsecondsToPulsewidth(pulse);
	Serial.println(duty);
    
	ledcWrite(servoChannel,duty);
}



