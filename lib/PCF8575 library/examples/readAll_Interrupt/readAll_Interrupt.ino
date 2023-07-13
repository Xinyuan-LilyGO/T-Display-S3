/*
 Read all data after interrupt
 by Mischianti Renzo <http://www.mischianti.org>

 https://www.mischianti.org/2019/01/02/pcf8575-i2c-digital-i-o-expander-fast-easy-usage/
*/

#include "Arduino.h"
#include "PCF8575.h"

// For arduino uno only pin 1 and 2 are interrupted
#define ARDUINO_UNO_INTERRUPTED_PIN 2

// Function interrupt
void keyChangedOnPCF8575();

// Set i2c address
PCF8575 pcf8575(0x39, ARDUINO_UNO_INTERRUPTED_PIN, keyChangedOnPCF8575);
unsigned long timeElapsed;
void setup()
{
	Serial.begin(115200);

	pcf8575.pinMode(P0, INPUT);
	pcf8575.pinMode(P1, INPUT);
	pcf8575.pinMode(P2, INPUT);
	pcf8575.pinMode(P3, INPUT);
	pcf8575.begin();

	Serial.println("START");

	timeElapsed = millis();
}

bool keyChanged = false;
void loop()
{
	if (keyChanged){
		PCF8575::DigitalInput di = pcf8575.digitalReadAll();
		Serial.print("READ VALUE FROM PCF P1: ");
		Serial.print(di.p0);
		Serial.print(" - ");
		Serial.print(di.p1);
		Serial.print(" - ");
		Serial.print(di.p2);
		Serial.print(" - ");
		Serial.println(di.p3);
//		delay(5);
		keyChanged= false;
	}
}

void keyChangedOnPCF8575(){
	// Interrupt called (No Serial no read no wire in this function, and DEBUG disabled on PCF library)
	 keyChanged = true;
}
