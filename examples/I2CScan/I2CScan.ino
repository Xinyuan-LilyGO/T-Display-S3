/**
 * @file      I2CScan.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-06-30
 *
 */

#include <Arduino.h>
#include <Wire.h>


void deviceScan(TwoWire *_port, Stream *stream)
{
    uint8_t err, addr;
    int nDevices = 0;
    for (addr = 1; addr < 127; addr++) {
        _port->beginTransmission(addr);
        err = _port->endTransmission();
        if (err == 0) {
            stream->print("I2C device found at address 0x");
            if (addr < 16)
                stream->print("0");
            stream->print(addr, HEX);
            stream->println(" !");
            nDevices++;
        } else if (err == 4) {
            stream->print("Unknow error at address 0x");
            if (addr < 16)
                stream->print("0");
            stream->println(addr, HEX);
        }
    }
    if (nDevices == 0)
        stream->println("No I2C devices found\n");
    else
        stream->println("Done\n");
}

int sda = 18;
int scl = 17;
#define PIN_POWER_ON 15

void setup()
{
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    Serial.begin(115200);
    Wire.begin(sda, scl);
}

void loop()
{
    deviceScan(&Wire, &Serial);
    delay(1000);
}