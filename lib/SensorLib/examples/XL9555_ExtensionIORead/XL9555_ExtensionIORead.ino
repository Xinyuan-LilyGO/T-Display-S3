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
 * @file      XL9555_ExtensionIORead.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2023-01-03
 *
 */
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <time.h>
#include "ExtensionIOXL9555.hpp"

#ifndef SENSOR_SDA
#define SENSOR_SDA  17
#endif

#ifndef SENSOR_SCL
#define SENSOR_SCL  18
#endif

#ifndef SENSOR_IRQ
#define SENSOR_IRQ  -1
#endif

ExtensionIOXL9555 extIO;

void setup()
{
    Serial.begin(115200);
    while (!Serial);


    // Device address 0x20~0x27
    if (!extIO.begin(Wire, XL9555_SLAVE_ADDRESS4, SENSOR_SDA, SENSOR_SCL)) {
        Serial.println("Failed to find XL9555 - check your wiring!");
        while (1) {
            delay(1000);
        }
    }

    pinMode(SENSOR_IRQ, INPUT_PULLUP);
    // Set PORT0 as input,mask = 0xFF = all pin input
    extIO.configPort(ExtensionIOXL9555::PORT0, 0xFF);
    // Set PORT1 as input,mask = 0xFF = all pin input
    extIO.configPort(ExtensionIOXL9555::PORT1, 0xFF);
}

void loop()
{
    Serial.print("PORT0:0b");
    Serial.print(extIO.readPort(ExtensionIOXL9555::PORT0), BIN);
    Serial.print("\tPORT1:0b");
    Serial.println(extIO.readPort(ExtensionIOXL9555::PORT1), BIN);

    //Allowable range of parameter input: IO0 ~ IO15
    Serial.println("IO0:\tIO1:\tIO2:\tIO3:\tIO4:\tIO5:\tIO6:\tIO7:\tIO8:\tIO9:\tIO10:\tIO11:\tIO12:\tIO13:\tIO14:\tIO15:\t");
    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO0));
    Serial.print("\t");
    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO1));
    Serial.print("\t");
    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO2));
    Serial.print("\t");
    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO3));
    Serial.print("\t");
    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO4));
    Serial.print("\t");
    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO5));
    Serial.print("\t");
    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO6));
    Serial.print("\t");
    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO7));
    Serial.print("\t");

    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO8));
    Serial.print("\t");
    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO9));
    Serial.print("\t");
    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO10));
    Serial.print("\t");
    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO11));
    Serial.print("\t");
    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO12));
    Serial.print("\t");
    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO13));
    Serial.print("\t");
    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO14));
    Serial.print("\t");
    Serial.print(extIO.digitalRead(ExtensionIOXL9555::IO15));
    Serial.println();
    delay(1000);
}



