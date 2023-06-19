/**
 * @file      SerialExample.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-06-19
 * @note      Serial usage example
 */

#include <Arduino.h>


const uint8_t rx1_pin = 17;
const uint8_t tx1_pin = 18;


const uint8_t rx2_pin = 21;
const uint8_t tx2_pin = 16;

void setup()
{

    // Arduino IDE USB_CDC_ON_BOOT = Eanble ,default Serial input & output data from USB-C
    // Arduino IDE USB_CDC_ON_BOOT = False  ,default Serial input & output data from IO43,IO44
    Serial.begin(115200);

    // Remap the input and output of Serial1
    Serial1.begin(9600, SERIAL_8N1, rx1_pin, tx1_pin);

    // Remap the input and output of Serial2
    Serial2.begin(9600, SERIAL_8N1, rx2_pin, tx2_pin);

}


void loop()
{

    while (Serial1.available()) {
        Serial.write(Serial1.read());
    }

    while (Serial2.available()) {
        Serial.write(Serial2.read());
    }

    while (Serial.available()) {
        Serial1.write(Serial.read());
    }

    delay(1);
}