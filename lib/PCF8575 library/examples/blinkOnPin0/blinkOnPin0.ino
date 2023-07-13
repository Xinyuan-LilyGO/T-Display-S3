/*
 Blink led on PIN0
 by Mischianti Renzo <http://www.mischianti.org>

 https://www.mischianti.org/2019/01/02/pcf8575-i2c-digital-i-o-expander-fast-easy-usage/
*/

#include "Arduino.h"
#include "PCF8575.h"

// Set i2c address
PCF8575 pcf8575(0x20);

void setup()
{
	Serial.begin(115200);

	// Set pinMode to OUTPUT
	pcf8575.pinMode(P0, OUTPUT);

	pcf8575.begin();
}

void loop()
{
	pcf8575.digitalWrite(P0, HIGH);
	delay(1000);
	pcf8575.digitalWrite(P0, LOW);
	delay(1000);
}
