/**
 *
 * @license MIT License
 *
 * Copyright (c) 2022 lewis he
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      GT911_GetPoint.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2023-04-12
 *
 */
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include "TouchDrvGT911.hpp"

#ifndef SENSOR_SDA
#define SENSOR_SDA  2
#endif

#ifndef SENSOR_SCL
#define SENSOR_SCL  3
#endif

#ifndef SENSOR_IRQ
#define SENSOR_IRQ  1
#endif

#ifndef SENSOR_RST
#define SENSOR_RST  10
#endif

TouchDrvGT911 touch;
int16_t x[5], y[5];



void setup()
{
    Serial.begin(115200);
    while (!Serial);

    Wire.begin(SENSOR_SDA, SENSOR_SCL);
    /*
    * The touch reset pin uses hardware pull-up,
    * and the function of setting the I2C device address cannot be used.
    * Use scanning to obtain the touch device address.
    * * */
    uint8_t touchAddress = 0;
    Wire.beginTransmission(0x14);
    if (Wire.endTransmission() == 0) {
        touchAddress = 0x14;
    }
    Wire.beginTransmission(0x5D);
    if (Wire.endTransmission() == 0) {
        touchAddress = 0x5D;
    }
    if (touchAddress == 0) {
        while (1) {
            Serial.println("Failed to find GT911 - check your wiring!");
            delay(1000);
        }
    }

    touch.setPins(SENSOR_RST, SENSOR_IRQ);

    if (!touch.begin(Wire,  touchAddress, SENSOR_SDA, SENSOR_SCL )) {
        while (1) {
            Serial.println("Failed to find GT911 - check your wiring!");
            delay(1000);
        }
    }

    //Set to trigger on falling edge
    touch.setInterruptMode(FALLING);

    Serial.println("Init GT911 Sensor success!");

}

void loop()
{
    if (touch.isPressed()) {
        uint8_t touched = touch.getPoint(x, y, touch.getSupportTouchPoint());
        for (int i = 0; i < touched; ++i) {
            Serial.print("X[");
            Serial.print(i);
            Serial.print("]:");
            Serial.print(x[i]);
            Serial.print(" ");
            Serial.print(" Y[");
            Serial.print(i);
            Serial.print("]:");
            Serial.print(y[i]);
            Serial.print(" ");
        }
        Serial.println();
    }
    delay(5);
}



