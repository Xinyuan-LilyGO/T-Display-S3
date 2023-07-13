/*
 * PCF8575 GPIO Port Expand
 * http://nopnop2002.webcrow.jp/WeMos/WeMos-25.html
 *
 * PCF8575    ----- WeMos
 * A0         ----- GRD
 * A1         ----- GRD
 * A2         ----- GRD
 * VSS        ----- GRD
 * VDD        ----- 5V/3.3V
 * SDA        ----- GPIO_4
 * SCL        ----- GPIO_5
 * INT        ----- GPIO_13
 *
 * P0     ----------------- BUTTON0
 * P1     ----------------- BUTTON1
 * P2     ----------------- BUTTON2
 * P3     ----------------- BUTTON3
 * P4     ----------------- BUTTON4
 * P5     ----------------- BUTTON5
 * P6     ----------------- BUTTON6
 * P7     ----------------- BUTTON7
 *
 */

#include "Arduino.h"
#include "PCF8575.h"  // https://github.com/xreef/PCF8575_library

#define ESP8266_INTERRUPTED_PIN 13

// Set i2c address
PCF8575 pcf8575(0x20);

// Function interrupt
bool keyPressed = false;

void keyPressedOnPCF8575(){
//  Serial.println("keyPressedOnPCF8575");
  keyPressed = true;
}

void setup()
{
  Serial.begin(9600);
  pinMode(ESP8266_INTERRUPTED_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ESP8266_INTERRUPTED_PIN), keyPressedOnPCF8575, FALLING);

  for(int i=0;i<8;i++) {
    pcf8575.pinMode(i, INPUT);
  }
  pcf8575.begin();
}

void loop()
{
  if (keyPressed){
    PCF8575::DigitalInput val = pcf8575.digitalReadAll();
    if (val.p0==HIGH) Serial.println("KEY0 PRESSED");
    if (val.p1==HIGH) Serial.println("KEY1 PRESSED");
    if (val.p2==HIGH) Serial.println("KEY2 PRESSED");
    if (val.p3==HIGH) Serial.println("KEY3 PRESSED");
    if (val.p4==HIGH) Serial.println("KEY4 PRESSED");
    if (val.p5==HIGH) Serial.println("KEY5 PRESSED");
    if (val.p6==HIGH) Serial.println("KEY6 PRESSED");
    if (val.p7==HIGH) Serial.println("KEY7 PRESSED");
    if (val.p8==HIGH) Serial.println("KEY8 PRESSED");
    if (val.p9==HIGH) Serial.println("KEY9 PRESSED");
    if (val.p10==HIGH) Serial.println("KEY10 PRESSED");
    if (val.p11==HIGH) Serial.println("KEY11 PRESSED");
    if (val.p12==HIGH) Serial.println("KEY12 PRESSED");
    if (val.p13==HIGH) Serial.println("KEY13 PRESSED");
    if (val.p14==HIGH) Serial.println("KEY14 PRESSED");
    if (val.p15==HIGH) Serial.println("KEY15 PRESSED");
    keyPressed= false;
  }
}
