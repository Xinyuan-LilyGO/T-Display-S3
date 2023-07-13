/*
 KeyPressed on PIN1
 by Mischianti Renzo <http://www.mischianti.org>

 https://www.mischianti.org/2019/01/02/pcf8575-i2c-digital-i-o-expander-fast-easy-usage/
*/

#include "Arduino.h"
#include "PCF8575.h"

// Set i2c address
PCF8575 pcf8575(0x20);
unsigned long timeElapsed;
void setup()
{
	Serial.begin(115200);

	pcf8575.pinMode(P1, INPUT);

	pcf8575.begin();
}

void loop()
{
	uint8_t val = pcf8575.digitalRead(P1);
	if (val==HIGH) Serial.println("KEY PRESSED");
	delay(50);
}
