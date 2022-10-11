#include "Arduino.h"
#include "OneButton.h"
#include "TFT_eSPI.h" /* Please use the TFT library provided in the library. */
#include "Wire.h"
#include "cstxx.h"
#include "img_logo.h"
#include "pin_config.h"

#warning Please confirm that you have purchased a display screen with a touch chip, otherwise the touch routine cannot be implemented.
CSTXXX touch(0, 0, 170, 320);
TFT_eSPI tft = TFT_eSPI();

bool flags_sleep = false;
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
  if (!touch.init(Wire, PIN_TOUCH_RES, PIN_TOUCH_INT)) {
    Serial.println("Touch IC not found");
  }

  button.attachClick([] { flags_sleep = 1; });
  tft.drawString("Press the BOT button to go", 0, 15);
  tft.drawString(" to sleep", 0, 40);
  // delay(500);
  // digitalWrite(PIN_POWER_ON, LOW);
  // gpio_hold_en((gpio_num_t)PIN_TOUCH_RES);
  // esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_TOUCH_INT, 0);
  // esp_deep_sleep_start();

  // attachInterrupt(
  //     PIN_TOUCH_INT,
  //     [] {
  //       Serial.println("get int");
  //       get_int_conut++;
  //     },
  //     FALLING);
}

void loop() {
  // touch_info_t t;
  // char str_buf[100];
  // static uint8_t last_finger;
  // if (touch.get_touch_point(&t)) {
  //   sprintf(str_buf, "finger num :%d \r\n", t.finger_num);
  //   tft.drawString(str_buf, 0, 15);
  //   for (uint8_t i = 0; i < t.finger_num; i++) {
  //     sprintf(str_buf, "x:%04d y:%04d p:%04d \r\n", t.point[i].x, t.point[i].y, t.point[i].pressure);
  //     tft.drawString(str_buf, 0, 35 + (20 * i));
  //   }
  //   if (last_finger > t.finger_num) {
  //     tft.fillRect(0, 55, 320, 100, TFT_BLACK);
  //   }
  //   last_finger = t.finger_num;
  // } else {
  //   tft.fillScreen(TFT_BLACK);
  // }
  // sprintf(str_buf, "Int count :%d \r\n", get_int_conut);
  // tft.drawString(str_buf, 0, 15);
  button.tick();
  if (flags_sleep) {
    flags_sleep = false;
    tft.fillScreen(TFT_BLACK);
    tft.drawString("about to go to sleep", 0, 15);
    touch.sleep();
    delay(2000);
    digitalWrite(PIN_POWER_ON, LOW);
    gpio_hold_en((gpio_num_t)PIN_TOUCH_RES);
    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_TOUCH_INT, 0);
    // esp_sleep_enable_ext1_wakeup((gpio_num_t)PIN_BUTTON_1, ESP_EXT1_WAKEUP_ALL_LOW);
    esp_deep_sleep_start();
  }
  delay(1);
}