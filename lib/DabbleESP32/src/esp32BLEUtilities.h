#if (defined(ESP32))
#ifndef BLE_UTILITIES_H
#define BLE_UTILITIES_H
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define UUID_Service  "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define UUID_Transmit "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define UUID_Receive  "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"



class Esp32ble
{
public:
void begin(std::string a);
void write(uint8_t a);
void write(std::string x);
void write(int a);
void write(float a);
uint8_t available();
uint8_t read();
void stop();
};
extern bool BLE_status;        //extern BLE_status
extern Esp32ble esp32ble;
#endif
#endif
