/**
 * @file      CapacitiveTouch.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @license   MIT
 * @copyright Copyright (c) 2024  Shenzhen Xin Yuan Electronic Technology Co., Ltd
 * @date      2024-05-15
 *
 */
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include <TouchDrvCSTXXX.hpp>
#include <TFT_eSPI.h>
#include "image.h"
#define PIN_LCD_BL                   38
#define PIN_LCD_D0                   39
#define PIN_LCD_D1                   40
#define PIN_LCD_D2                   41
#define PIN_LCD_D3                   42
#define PIN_LCD_D4                   45
#define PIN_LCD_D5                   46
#define PIN_LCD_D6                   47
#define PIN_LCD_D7                   48
#define PIN_POWER_ON                 15
#define PIN_LCD_RES                  5
#define PIN_LCD_CS                   6
#define PIN_LCD_DC                   7
#define PIN_LCD_WR                   8
#define PIN_LCD_RD                   9
#define PIN_BUTTON_1                 0
#define PIN_BUTTON_2                 14
#define PIN_BAT_VOLT                 4
#define BOARD_I2C_SCL                  17
#define BOARD_I2C_SDA                  18
#define BOARD_TOUCH_IRQ                16
#define BOARD_TOUCH_RST                21



TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

TouchDrvCSTXXX touch;
int16_t x[5], y[5];

void setup()
{
    Serial.begin(115200);

    // Initialize screen , The backlight Pin has been configured in TFT_eSPI(TFT_eSPI/User_Setups/Setup214_LilyGo_T_Display_S3_Pro.h).
    // By default, the backlight is turned on after initializing the TFT.
    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);

    // Initialize capacitive touch
    touch.setPins(BOARD_TOUCH_RST, BOARD_TOUCH_IRQ);

    if (!touch.begin(Wire, CST328_SLAVE_ADDRESS, BOARD_I2C_SDA, BOARD_I2C_SCL)) {
        Serial.println("Failed init CST328 Device!");
        if (!touch.begin(Wire, CST816_SLAVE_ADDRESS, BOARD_I2C_SDA, BOARD_I2C_SCL)) {
            Serial.println("Failed init CST816 Device!");
            while (1) {
                Serial.println("Not find touch device!"); delay(1000);
            }
        }
    }
    // Press the circular touch button on the screen to get the coordinate point,
    // and set it as the coordinate of the touch button
    // T-Display-S3 CST816 touch panel, touch button coordinates are is 85 , 160
    touch.setCenterButtonCoordinate(85, 360);

    // Depending on the touch panel, not all touch panels have touch buttons.
    touch.setHomeButtonCallback([](void *user_data) {
        Serial.println("Home key pressed!");
        static uint32_t checkMs = 0;
        if (millis() > checkMs) {
            if (digitalRead(TFT_BL)) {
                digitalWrite(TFT_BL, LOW);
            } else {
                digitalWrite(TFT_BL, HIGH);
            }
        }
        checkMs = millis() + 200;
    }, NULL);

    // If you poll the touch, you need to turn off the automatic sleep function, otherwise there will be an I2C access error.
    // If you use the interrupt method, you don't need to turn it off, saving power consumption
    touch.disableAutoSleep();

    // tft.setSwapBytes(true);
    tft.pushColors((uint16_t *)image, tft.width() * tft.height(), true);

    spr.setColorDepth(8);
    spr.createSprite(tft.width(), 60);
    spr.fillSprite(TFT_LIGHTGREY);
    spr.setTextDatum(CL_DATUM);
    spr.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
    spr.drawString("Powered By LilyGo ST7796 320x170 IPS TFT", 5, 10, 2);
    spr.pushSprite(0, tft.height() - 60);

}

void loop()
{
    uint8_t touched = touch.getPoint(x, y, touch.getSupportTouchPoint());
    if (touched) {
        String buf = "";
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

            buf += "[" + String(i) + "]:";
            buf += String(x[i]);
            buf += " ";
            buf += " [" + String(i) + "]:";
            buf += String(y[i]);
            buf += " ";
        }
        Serial.println();
        spr.drawString("Powered By LilyGo ST7789 320x170 IPS TFT", 5, 10, 2);
        spr.drawString(buf, 5, 30, 2);
        spr.pushSprite(0, tft.height() - 60);
        spr.fillSprite(TFT_LIGHTGREY);
    }

    delay(5);
}



// TFT Pin check
#if PIN_LCD_WR  != TFT_WR || \
    PIN_LCD_RD  != TFT_RD || \
    PIN_LCD_CS    != TFT_CS   || \
    PIN_LCD_DC    != TFT_DC   || \
    PIN_LCD_RES   != TFT_RST  || \
    PIN_LCD_D0   != TFT_D0  || \
    PIN_LCD_D1   != TFT_D1  || \
    PIN_LCD_D2   != TFT_D2  || \
    PIN_LCD_D3   != TFT_D3  || \
    PIN_LCD_D4   != TFT_D4  || \
    PIN_LCD_D5   != TFT_D5  || \
    PIN_LCD_D6   != TFT_D6  || \
    PIN_LCD_D7   != TFT_D7  || \
    PIN_LCD_BL   != TFT_BL  || \
    TFT_BACKLIGHT_ON   != HIGH  || \
    170   != TFT_WIDTH  || \
    320   != TFT_HEIGHT
#error  "Error! Please make sure <User_Setups/Setup206_LilyGo_T_Display_S3.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#error  "Error! Please make sure <User_Setups/Setup206_LilyGo_T_Display_S3.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#error  "Error! Please make sure <User_Setups/Setup206_LilyGo_T_Display_S3.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#error  "Error! Please make sure <User_Setups/Setup206_LilyGo_T_Display_S3.h> is selected in <TFT_eSPI/User_Setup_Select.h>"
#endif

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
#error  "The current version is not supported for the time being, please use a version below Arduino ESP32 3.0"
#endif