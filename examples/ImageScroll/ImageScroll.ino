/*
 * @file      ImageScroll.ino
 * @author    Rudi Ackermann (rudi.ackermann@hotmail.de)
 * @license   MIT
 * @copyright Copyright (c) 2023 Rudi Ackermann
 * @date      2023-08-17
 * @note      Sketch Adaptation for T-Display S3
*/


#include "TFT_eSPI.h"
//#include "World_Time_Zones.h"
#include "Germany.h"
#include "pin_config.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);


//int imageW=480;
//int imageH=270;
int imageW = 350;
int imageH = 478;
int screenW = 320;
int screenH = 170;
int m = imageW;

int start = 1;
unsigned short imageS[54400] = {0}; // edit this screenW * screen H


void setup()
{
    //Turn on display power
    pinMode(15, OUTPUT);
    digitalWrite(15, HIGH);

    tft.init();
    tft.setRotation(1);

    sprite.createSprite(320, 170);
    sprite.setSwapBytes(true);

}
int pos = 0;
int x = 0;
int y = 30;
int changeX = 1;
int changeY = 1;
void loop()
{

    pos = x + imageW * y;
    start = pos;
    m = screenW + pos;
    for (int i = 0; i < screenW * screenH; i++) {
        if (start % m == 0) {
            start = start + (imageW - screenW);
            m = m + imageW;
        }
        imageS[i] = Germany[start];
        start++;

    }
    x = x + changeX;
    if (x == imageW - screenW - 1 || x < 0)
        changeX = changeX * -1;

    y = y + changeY;
    if (y == imageH - screenH - 1 || y < 0)
        changeY = changeY * -1;

    sprite.pushImage(0, 0, screenW, screenH, imageS);
    sprite.pushSprite(0, 0);


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
