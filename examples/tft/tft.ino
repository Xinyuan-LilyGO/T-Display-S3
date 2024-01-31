#include "Arduino.h"
#include "TFT_eSPI.h" /* Please use the TFT library provided in the library. */
#include "img_logo.h"
#include "pin_config.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
#error  "The current version is not supported for the time being, please use a version below Arduino ESP32 3.0"
#endif

/* The product now has two screens, and the initialization code needs a small change in the new version. The LCD_MODULE_CMD_1 is used to define the
 * switch macro. */
#define LCD_MODULE_CMD_1

TFT_eSPI tft = TFT_eSPI();
#define WAIT 1000
unsigned long targetTime = 0; // Used for testing draw times

#if defined(LCD_MODULE_CMD_1)
typedef struct {
    uint8_t cmd;
    uint8_t data[14];
    uint8_t len;
} lcd_cmd_t;

lcd_cmd_t lcd_st7789v[] = {
    {0x11, {0}, 0 | 0x80},
    {0x3A, {0X05}, 1},
    {0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5},
    {0xB7, {0X75}, 1},
    {0xBB, {0X28}, 1},
    {0xC0, {0X2C}, 1},
    {0xC2, {0X01}, 1},
    {0xC3, {0X1F}, 1},
    {0xC6, {0X13}, 1},
    {0xD0, {0XA7}, 1},
    {0xD0, {0XA4, 0XA1}, 2},
    {0xD6, {0XA1}, 1},
    {0xE0, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14},
    {0xE1, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14},
};
#endif

void setup()
{
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);

    Serial.begin(115200);
    Serial.println("Hello T-Display-S3");

    tft.begin();

#if defined(LCD_MODULE_CMD_1)
    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++) {
        tft.writecommand(lcd_st7789v[i].cmd);
        for (int j = 0; j < (lcd_st7789v[i].len & 0x7f); j++) {
            tft.writedata(lcd_st7789v[i].data[j]);
        }

        if (lcd_st7789v[i].len & 0x80) {
            delay(120);
        }
    }
#endif


    tft.setRotation(3);
    tft.setSwapBytes(true);
    tft.pushImage(0, 0, 320, 170, (uint16_t *)img_logo);
    delay(2000);

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5,0,0)
    ledcSetup(0, 2000, 8);
    ledcAttachPin(PIN_LCD_BL, 0);
    ledcWrite(0, 255);
#else
    ledcAttach(PIN_LCD_BL, 200, 8);
    ledcWrite(PIN_LCD_BL, 255);
#endif
}

void loop()
{
    targetTime = millis();

    // First we test them with a background colour set
    tft.setTextSize(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);

    tft.drawString(" !\"#$%&'()*+,-./0123456", 0, 0, 2);
    tft.drawString("789:;<=>?@ABCDEFGHIJKL", 0, 16, 2);
    tft.drawString("MNOPQRSTUVWXYZ[\\]^_`", 0, 32, 2);
    tft.drawString("abcdefghijklmnopqrstuvw", 0, 48, 2);
    int xpos = 0;
    xpos += tft.drawString("xyz{|}~", 0, 64, 2);
    tft.drawChar(127, xpos, 64, 2);
    delay(WAIT);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN, TFT_BLACK);

    tft.drawString(" !\"#$%&'()*+,-.", 0, 0, 4);
    tft.drawString("/0123456789:;", 0, 26, 4);
    tft.drawString("<=>?@ABCDE", 0, 52, 4);
    tft.drawString("FGHIJKLMNO", 0, 78, 4);
    tft.drawString("PQRSTUVWX", 0, 104, 4);
    delay(WAIT);

    tft.fillScreen(TFT_BLACK);
    tft.drawString("YZ[\\]^_`abc", 0, 0, 4);
    tft.drawString("defghijklmno", 0, 26, 4);
    tft.drawString("pqrstuvwxyz", 0, 52, 4);
    xpos = 0;
    xpos += tft.drawString("{|}~", 0, 78, 4);
    tft.drawChar(127, xpos, 78, 4);
    delay(WAIT);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_BLUE, TFT_BLACK);

    tft.drawString("012345", 0, 0, 6);
    tft.drawString("6789", 0, 40, 6);
    tft.drawString("apm-:.", 0, 80, 6);
    delay(WAIT);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_RED, TFT_BLACK);

    tft.drawString("0123", 0, 0, 7);
    tft.drawString("4567", 0, 60, 7);
    delay(WAIT);

    tft.fillScreen(TFT_BLACK);
    tft.drawString("890:.", 0, 0, 7);
    tft.drawString("", 0, 60, 7);
    delay(WAIT);

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);

    tft.drawString("01", 0, 0, 8);
    delay(WAIT);

    tft.drawString("23", 0, 0, 8);
    delay(WAIT);

    tft.drawString("45", 0, 0, 8);
    delay(WAIT);

    tft.drawString("67", 0, 0, 8);
    delay(WAIT);

    tft.drawString("89", 0, 0, 8);
    delay(WAIT);

    tft.drawString("0:.", 0, 0, 8);
    delay(WAIT);

    tft.setTextColor(TFT_MAGENTA);
    tft.drawNumber(millis() - targetTime, 0, 100, 4);
    delay(4000);

    // Now test them with transparent background
    targetTime = millis();

    tft.setTextSize(1);
    tft.fillScreen(TFT_BROWN);
    tft.setTextColor(TFT_GREEN);

    tft.drawString(" !\"#$%&'()*+,-./0123456", 0, 0, 2);
    tft.drawString("789:;<=>?@ABCDEFGHIJKL", 0, 16, 2);
    tft.drawString("MNOPQRSTUVWXYZ[\\]^_`", 0, 32, 2);
    tft.drawString("abcdefghijklmnopqrstuvw", 0, 48, 2);
    xpos = 0;
    xpos += tft.drawString("xyz{|}~", 0, 64, 2);
    tft.drawChar(127, xpos, 64, 2);
    delay(WAIT);

    tft.fillScreen(TFT_BROWN);
    tft.setTextColor(TFT_GREEN);

    tft.drawString(" !\"#$%&'()*+,-.", 0, 0, 4);
    tft.drawString("/0123456789:;", 0, 26, 4);
    tft.drawString("<=>?@ABCDE", 0, 52, 4);
    tft.drawString("FGHIJKLMNO", 0, 78, 4);
    tft.drawString("PQRSTUVWX", 0, 104, 4);

    delay(WAIT);
    tft.fillScreen(TFT_BROWN);
    tft.drawString("YZ[\\]^_`abc", 0, 0, 4);
    tft.drawString("defghijklmno", 0, 26, 4);
    tft.drawString("pqrstuvwxyz", 0, 52, 4);
    xpos = 0;
    xpos += tft.drawString("{|}~", 0, 78, 4);
    tft.drawChar(127, xpos, 78, 4);
    delay(WAIT);

    tft.fillScreen(TFT_BROWN);
    tft.setTextColor(TFT_BLUE);

    tft.drawString("012345", 0, 0, 6);
    tft.drawString("6789", 0, 40, 6);
    tft.drawString("apm-:.", 0, 80, 6);
    delay(WAIT);

    tft.fillScreen(TFT_BROWN);
    tft.setTextColor(TFT_RED);

    tft.drawString("0123", 0, 0, 7);
    tft.drawString("4567", 0, 60, 7);
    delay(WAIT);

    tft.fillScreen(TFT_BROWN);
    tft.drawString("890:.", 0, 0, 7);
    tft.drawString("", 0, 60, 7);
    delay(WAIT);

    tft.fillScreen(TFT_BROWN);
    tft.setTextColor(TFT_YELLOW);

    tft.drawString("0123", 0, 0, 8);
    delay(WAIT);

    tft.fillScreen(TFT_BROWN);
    tft.drawString("4567", 0, 0, 8);
    delay(WAIT);

    tft.fillScreen(TFT_BROWN);
    tft.drawString("890:.", 0, 0, 8);
    delay(WAIT);

    tft.setTextColor(TFT_MAGENTA);

    tft.drawNumber(millis() - targetTime, 0, 100, 4);
    delay(4000);
}
