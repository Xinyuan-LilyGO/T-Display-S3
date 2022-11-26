#define TOUCH_MODULES_GT911
//#define TOUCH_MODULES_CST_SELF
//#define TOUCH_MODULES_CST_MUTUAL
//#define TOUCH_MODULES_ZTW622
//#define TOUCH_MODULES_L58

#include "Arduino.h"
#include "TouchLib.h"
#include "Wire.h"

#define PIN_IIC_SCL 48
#define PIN_IIC_SDA 8
#define PIN_TOUCH_INT 1
#define PIN_TOUCH_RES 2

#define TOUCH_READ_FROM_INTERRNUPT 0

TouchLib touch(Wire, PIN_IIC_SDA, PIN_IIC_SCL, GT911_SLAVE_ADDRESS2);

void scan_i2c_device(TwoWire &i2c)
{
  Serial.println("Scanning for I2C devices ...");
  Serial.print("      ");
  for (int i = 0; i < 0x10; i++)
  {
    Serial.printf("0x%02X|", i);
  }
  uint8_t error;
  for (int j = 0; j < 0x80; j += 0x10)
  {
    Serial.println();
    Serial.printf("0x%02X |", j);
    for (int i = 0; i < 0x10; i++)
    {
      Wire.beginTransmission(i | j);
      error = Wire.endTransmission();
      if (error == 0)
        Serial.printf("0x%02X|", i | j);
      else
        Serial.print(" -- |");
    }
  }
  Serial.println();
}

bool get_int = false;
void setup()
{
  Serial.begin(115200);
  Serial.println("Hello T-Display-S3");
  pinMode(PIN_TOUCH_RES, OUTPUT);
  digitalWrite(PIN_TOUCH_RES, 0);
  delay(200);
  digitalWrite(PIN_TOUCH_RES, 1);
  delay(200);
  Wire.begin(PIN_IIC_SDA, PIN_IIC_SCL);
  scan_i2c_device(Wire);

  touch.init();
#if (TOUCH_READ_FROM_INTERRNUPT)
  attachInterrupt(
      PIN_TOUCH_INT,
      []
      {
        get_int = 1;
        Serial.println("get_int");
      },
      CHANGE);
#endif
}

void loop()
{
  delay(5);
#if (TOUCH_READ_FROM_INTERRNUPT)
  if (get_int)
  {
    get_int = 0;
    touch.read();
#else
  if (touch.read())
  {
#endif
    uint8_t n = touch.getPointNum();
    Serial.printf("getPointNum: %d  ", n);
    for (uint8_t i = 0; i < n; i++)
    {
      TP_Point t = touch.getPoint(i);
      Serial.printf("[%d] point x: %d  point y: %d \r\n", i, t.x, t.y);
    }
  }
}
