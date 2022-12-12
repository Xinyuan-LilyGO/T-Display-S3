/* Please make sure your touch IC model. */
#define TOUCH_MODULES_CST_MUTUAL
// #define TOUCH_MODULES_CST_SELF
#include "TouchLib.h"

#if ARDUINO_USB_MODE
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else

#include "Arduino.h"
#include "OneButton.h"
#include "TFT_eSPI.h" /* Please use the TFT library provided in the library. */
#include "Wire.h"
#include "img_logo.h"
#include "pin_config.h"

#include "USB.h"
#include "USBHID.h"
#include "USBHIDMouse.h"

#warning Please confirm that you have purchased a display screen with a touch chip, otherwise the touch routine cannot be implemented.
TouchLib touch(Wire, PIN_IIC_SDA, PIN_IIC_SCL, CTS328_SLAVE_ADDRESS);

#define TOUCH_GET_FORM_INT 0

USBHIDMouse Mouse;
TFT_eSPI tft = TFT_eSPI();

#if TOUCH_GET_FORM_INT
bool get_int = false;
#endif

OneButton left_button(PIN_BUTTON_1, true);
OneButton right_button(PIN_BUTTON_2, true);
void setup() {
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
  
  ledcSetup(0, 2000, 8);
  ledcAttachPin(PIN_LCD_BL, 0);
  ledcWrite(0, 255);

  delay(2000);

  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);

  Wire.begin(PIN_IIC_SDA, PIN_IIC_SCL);
  if (!touch.init()) {
    Serial.println("Touch IC not found");
  }
  touch.setRotation(1);

  left_button.attachClick([] { Mouse.click(MOUSE_LEFT); });
  right_button.attachClick([] { Mouse.click(MOUSE_RIGHT); });
  tft.drawString("Please touch the screen to ", 0, 15);
  tft.drawString("simulate mouse sliding.", 0, 40);
#if TOUCH_GET_FORM_INT
  attachInterrupt(
      PIN_TOUCH_INT, [] { get_int = true; }, FALLING);
#endif

  Mouse.begin();
  USB.begin();
}

void loop() {
  char str_buf[50];
  static bool first_touch;
  static uint16_t last_x, last_y;
  static uint32_t Mill;
#if TOUCH_GET_FORM_INT
  if (get_int) {
    get_int = 0;
    touch.read();
#else
  if (touch.read()) {
#endif
    uint8_t n = touch.getPointNum();
    TP_Point t = touch.getPoint(0);
    if (!first_touch) {
      Mouse.move(int(t.x - last_x), int(t.y - last_y));
    }
    last_x = t.x;
    last_y = t.y;
    first_touch = 0;
    Mill = millis();
  } else {
    if (millis() - Mill > 50)
      first_touch = 1;
  }

  left_button.tick();
  right_button.tick();
  delay(5);
}

#endif