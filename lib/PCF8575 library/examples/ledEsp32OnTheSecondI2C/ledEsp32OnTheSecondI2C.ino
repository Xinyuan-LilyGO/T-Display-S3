#include "Arduino.h"
/*
 * 	PCF8575 GPIO Port Expand
 *  Blink all led
 *  by Mischianti Renzo <http://www.mischianti.org>
 *
 *  https://www.mischianti.org/2019/01/02/pcf8575-i2c-digital-i-o-expander-fast-easy-usage/
 *
 *
 * PCF8575    ----- Esp32
 * A0         ----- GRD
 * A1         ----- GRD
 * A2         ----- GRD
 * VSS        ----- GRD
 * VDD        ----- 5V/3.3V
 * SDA        ----- 21
 * SCL        ----- 22
 *
 * P0     ----------------- LED0
 * P1     ----------------- LED1
 * P2     ----------------- LED2
 * P3     ----------------- LED3
 * P4     ----------------- LED4
 * P5     ----------------- LED5
 * P6     ----------------- LED6
 * P7     ----------------- LED7
 *
 */

#include "Arduino.h"
#include "PCF8575.h"  // https://github.com/xreef/PCF8575_library

// Instantiate Wire for generic use at 400kHz
TwoWire I2Cone = TwoWire(0);
// Instantiate Wire for generic use at 100kHz
TwoWire I2Ctwo = TwoWire(1);

// Set i2c address
PCF8575 pcf8575(&I2Ctwo, 0x20);
// PCF8575 pcf8575(&I2Ctwo, 0x20, 21, 22);
// PCF8575(TwoWire *pWire, uint8_t address, uint8_t interruptPin,  void (*interruptFunction)() );
// PCF8575(TwoWire *pWire, uint8_t address, uint8_t sda, uint8_t scl, uint8_t interruptPin,  void (*interruptFunction)());

void setup()
{
  Serial.begin(112560);

  I2Cone.begin(16,17,400000); // SDA pin 16, SCL pin 17, 400kHz frequency

  // Set pinMode to OUTPUT
  for(int i=0;i<8;i++) {
    pcf8575.pinMode(i, OUTPUT);
  }
  pcf8575.begin();
}

void loop()
{
  static int pin = 0;
  pcf8575.digitalWrite(pin, HIGH);
  delay(400);
  pcf8575.digitalWrite(pin, LOW);
  delay(400);
  pin++;
  if (pin > 7) pin = 0;
}
