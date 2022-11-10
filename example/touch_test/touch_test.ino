#define TOUCH_MODULES_CST_MUTUAL

#include "Arduino.h"
#include "OneButton.h"
#include "TFT_eSPI.h" /* Please use the TFT library provided in the library. */
#include "TouchLib.h"
#include "Wire.h"
#include "img_logo.h"
#include "pin_config.h"

#warning Please confirm that you have purchased a display screen with a touch chip, otherwise the touch routine cannot be implemented.
TouchLib touch(Wire, PIN_IIC_SDA, PIN_IIC_SCL, CTS328_SLAVE_ADDRESS);

#define TOUCH_GET_FORM_INT 0

TFT_eSPI tft = TFT_eSPI();
bool flags_sleep = false;
#if TOUCH_GET_FORM_INT
bool get_int = false;
#endif
OneButton button(PIN_BUTTON_1, true);

void setup() {
  gpio_hold_dis((gpio_num_t)PIN_TOUCH_RES);

  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);

  pinMode(PIN_TOUCH_RES, OUTPUT);
  digitalWrite(PIN_TOUCH_RES, LOW);
  delay(500);
  digitalWrite(PIN_TOUCH_RES, HIGH);

  Serial.begin(115200);
  Serial.println("Hello T-Display-S3");

  tft.begin();
  tft.setRotation(3);
  tft.pushImage(0, 0, 320, 170, (uint16_t *)img_logo);
  delay(2000);
  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  Wire.begin(PIN_IIC_SDA, PIN_IIC_SCL);
  if (!touch.init()) {
    Serial.println("Touch IC not found");
  }

  button.attachClick([] { flags_sleep = 1; });
  tft.drawString("Press the BOT button to go", 0, 15);
  tft.drawString(" to sleep", 0, 40);
#if TOUCH_GET_FORM_INT
  attachInterrupt(
      PIN_TOUCH_INT, [] { get_int = true; }, FALLING);
#endif
}

void loop() {
  char str_buf[100];
  static uint8_t last_finger;
#if TOUCH_GET_FORM_INT
  if (get_int) {
    get_int = 0;
    touch.read();
#else
  if (touch.read()) {
#endif
    uint8_t n = touch.getPointNum();
    sprintf(str_buf, "finger num :%d \r\n", n);
    tft.drawString(str_buf, 0, 15);
    for (uint8_t i = 0; i < n; i++) {
      TP_Point t = touch.getPoint(i);
      sprintf(str_buf, "x:%04d y:%04d p:%04d \r\n", t.x, t.y, t.pressure);
      tft.drawString(str_buf, 0, 35 + (20 * i));
    }
    if (last_finger > n) {
      tft.fillRect(0, 55, 320, 100, TFT_BLACK);
    }
    last_finger = n;
  } else {
    tft.fillScreen(TFT_BLACK);
  }

  button.tick();
  if (flags_sleep) {
    flags_sleep = false;
    tft.fillScreen(TFT_BLACK);
    tft.drawString("about to go to sleep", 0, 15);
    touch.enableSleep();
    delay(2000);
    digitalWrite(PIN_POWER_ON, LOW);
    gpio_hold_en((gpio_num_t)PIN_TOUCH_RES);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_TOUCH_INT, 0);
    esp_deep_sleep_start();
  }
  delay(1);
}