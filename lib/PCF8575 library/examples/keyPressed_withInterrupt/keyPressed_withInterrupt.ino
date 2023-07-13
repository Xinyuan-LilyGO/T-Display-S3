/*
 KeyPressed with interrupt
 by Mischianti Renzo <http://www.mischianti.org>

 https://www.mischianti.org/2019/01/02/pcf8575-i2c-digital-i-o-expander-fast-easy-usage/
*/

#include "Arduino.h"
#include "PCF8575.h"

// For arduino uno only pin 1 and 2 are interrupted
#define ARDUINO_UNO_INTERRUPTED_PIN 2

// Function interrupt
void keyPressedOnPCF8575();

// Set i2c address
PCF8575 pcf8575(0x39, ARDUINO_UNO_INTERRUPTED_PIN, keyPressedOnPCF8575);
unsigned long timeElapsed;
void setup()
{
	Serial.begin(115200);

	pcf8575.pinMode(P1, INPUT);
	pcf8575.begin();


	timeElapsed = millis();
}

bool keyPressed = false;
void loop()
{
	if (keyPressed){
		uint8_t val = pcf8575.digitalRead(P1);
		Serial.print("READ VALUE FROM PCF ");
		Serial.println(val);
		keyPressed= false;
	}
}

void keyPressedOnPCF8575(){
	// Interrupt called (No Serial no read no wire in this function, and DEBUG disabled on PCF library)
	 keyPressed = true;

}
