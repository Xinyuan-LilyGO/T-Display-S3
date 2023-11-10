//Example from: https://github.com/VolosR/PCBDesignClock

#include <TFT_eSPI.h>
#include <WiFi.h>
#include "time.h"
#include "fonts.h"
#include "background.h"
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

const char *ssid     = "WiFi SSID";
const char *password = "WIFI PASSWORD";

const char *ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;         //time zone * 3600 , my time zone is  +1 GTM
const int   daylightOffset_sec = 3600;

int frame = 0;
int err = 5;

char timeHour[3];
char timeMin[3];
char timeSec[3];
char day[3];
char month[10];
char year[5];
char timeWeekDay[10];
int dayInWeek;

int brightnesses[7] = {35, 70, 105, 140, 175, 210, 250};
int bright = 4;
int deb = 0;
int deb2 = 0;
bool scheme = 0;

unsigned long currTime = 0;
int period = 900;

void setup()
{
    pinMode(15, OUTPUT);
    digitalWrite(15, 1);

    pinMode(0, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);
    tft.init();
    tft.invertDisplay(1);
    tft.setRotation(1);



    sprite.createSprite(320, 170);
    sprite.setSwapBytes(true);
    sprite.setTextColor(TFT_WHITE, 0x014C);

    delay(50);
    ledcSetup(0, 10000, 8);
    ledcAttachPin(38, 0);
    ledcWrite(0, brightnesses[bright]);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    } configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void draw()
{
    sprite.fillSprite(TFT_BLACK);
    sprite.setTextColor(TFT_WHITE, frames[frame][0]);
    sprite.pushImage(0, 0, 320, 170, frames[frame]);

    sprite.setFreeFont(&DSEG7_Classic_Bold_34);
    sprite.drawString(String(timeHour) + ":" + String(timeMin), 12, 42);

    sprite.setFreeFont(&DSEG7_Classic_Regular_16);
    sprite.drawString(timeSec, 237, 134);

    sprite.setTextFont(0);

    sprite.drawString("0x" + String(frame, HEX), 176, 159);
    sprite.drawString(String(month) + " " + String(day), 141, 110);
    sprite.drawString(String(timeWeekDay), 210, 42);

    if (err >= 7) {
        sprite.setTextColor(0xB800, frames[frame][0]);
        sprite.drawString("ERROR", 6, 4);
    }

    for (int i = 0; i < bright; i++)
        sprite.fillCircle(304, 30 + (i * 8), 2, TFT_WHITE);

    sprite.pushSprite(0, 0);
}

void getTime()    // get time from server
{
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return;
    }

    strftime(timeHour, 3, "%H", &timeinfo);
    strftime(timeMin, 3, "%M", &timeinfo);
    strftime(timeSec, 3, "%S", &timeinfo);
    strftime(timeWeekDay, 10, "%A", &timeinfo);

    strftime(day, 3, "%d", &timeinfo);
    strftime(month, 10, "%B", &timeinfo);
    strftime(year, 5, "%Y", &timeinfo);
}

void loop()
{

    if (digitalRead(0) == 0) { //if left button pressed , change brightness
        if (deb == 0) {
            deb = 1; bright++; if (bright == 7) bright = 0;
            ledcWrite(0, brightnesses[bright]);
        }
    } else deb = 0;

    if (digitalRead(14) == 0) { //if left button pressed , change color
        if (deb2 == 0) {
            deb2 = 1;
            scheme = !scheme;
        }
    } else deb2 = 0;

    if (millis() > currTime + period) { //update time, and chose next frame every "period" of time
        currTime = millis(); getTime();
        frame = random(scheme * 11, (scheme + 1) * 11); err = random(0, 10);
    }
    draw();
}


