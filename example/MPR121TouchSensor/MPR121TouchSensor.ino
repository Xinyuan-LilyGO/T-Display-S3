/**
 * @file      MPR121.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2023  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2023-06-30
 * @note      MPR121 is an external access sensor, not included in T-Display S3
 */
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPR121.h>

Adafruit_MPR121 cap1 = Adafruit_MPR121();
Adafruit_MPR121 cap2 = Adafruit_MPR121();


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
    deviceScan(&Wire, &Serial);


    if (!cap1.begin(0x5A)) {
        Serial.println("MPR121 not found, check wiring?");
        while (1);
    }
    Serial.println("MPR121 found!");

    if (!cap2.begin(0x5B)) {
        Serial.println("MPR121 not found, check wiring?");
        while (1);
    }
    Serial.println("MPR121 found!");
}


bool getTouch(Adafruit_MPR121 &cap, uint8_t index)
{
    uint16_t lasttouched[2] = {0};
    uint16_t currtouched[2] = {0};
    // Get the currently touched pads
    currtouched[index] = cap.touched();

    for (uint8_t i = 0; i < 12; i++) {
        // it if *is* touched and *wasnt* touched before, alert!
        if ((currtouched[index] & _BV(i)) && !(lasttouched[index] & _BV(i)) ) {
            Serial.print("Cap"); Serial.print(index); Serial.print(" -> ");
            Serial.print(i); Serial.println(" touched");
        }
        // if it *was* touched and now *isnt*, alert!
        if (!(currtouched[index] & _BV(i)) && (lasttouched[index] & _BV(i)) ) {
            Serial.print("Cap"); Serial.print(index); Serial.print(" -> ");
            Serial.print(i); Serial.println(" released");
        }
    }
    // reset our state
    lasttouched[index] = currtouched[index];
    return true;
}

void loop()
{
    getTouch(cap1, 0);
    getTouch(cap2, 1);
    delay(1);
}
