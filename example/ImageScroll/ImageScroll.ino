/*
 * @file      ImageScroll.ino
 * @author    Rudi Ackermann (rudi.ackermann@hotmail.de)
 * @license   MIT
 * @copyright Copyright (c) 2023 Rudi Ackermann
 * @date      2023-08-17
 * @note      Sketch Adaptation for T-Display S3
*/
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
#error  "The current version is not supported for the time being, please use a version below Arduino ESP32 3.0"
#endif

#include "TFT_eSPI.h"
//#include "World_Time_Zones.h"
#include "Germany.h"
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
