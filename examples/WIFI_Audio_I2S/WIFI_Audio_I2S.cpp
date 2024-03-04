/*
 * @Description: WIFI_Audio_I2S test

 * @version: V1.0.0
 * @Author: None
 * @Date: 2023-06-12 14:27:51
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2024-03-04 16:43:25
 * @License: GPL 3.0
 */
#include <Arduino.h>
#include "WiFiMulti.h"
#include "Audio.h"
#include "SPI.h"
#include "SD.h"
#include "FS.h"
#include "TFT_eSPI.h"
#include "pin_config.h"

// IIS 1
#define PIN_PCM5102A_DIN 43
#define PIN_PCM5102A_BCLK 44
#define PIN_PCM5102A_LRCLK 18
// IIS 2
// #define PIN_PCM5102A_DIN 11
// #define PIN_PCM5102A_BCLK 12
// #define PIN_PCM5102A_LRCLK 13

Audio audio;
TFT_eSPI tft;
String ssid = "LilyGo-AABB";
String password = "xinyuandianzi";

void setup()
{
    pinMode(15 /* PWD */, OUTPUT);
    digitalWrite(15 /* PWD */, HIGH);

    Serial.begin(115200);
    Serial.println("Hello T-Display-S3");

    tft.begin();
    tft.setRotation(3);
    tft.fillScreen(TFT_BLACK);

    ledcAttachPin(TFT_BL, 1);
    ledcSetup(1, 2000, 8);
    ledcWrite(1, 255);

    WiFi.disconnect();
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());
    delay(2000);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("WiFi connection failed");
        tft.println("WiFi connection failed");
        delay(1000);
    }

    Serial.println("WiFi connection successful");
    tft.println("WiFi connection successful");

    audio.setPinout(PIN_PCM5102A_BCLK, PIN_PCM5102A_LRCLK, PIN_PCM5102A_DIN);
    audio.setVolume(10); // 0...21,Volume setting
                         // audio.setBalance(-16);

    Serial.println("Trying to play music...");
    tft.println("Trying to play music...");

    audio.connecttohost("http://music.163.com/song/media/outer/url?id=29747854.mp3"); // MP3
}

void loop()
{
    audio.loop();
}

// optional
void audio_info(const char *info)
{
    Serial.print("info        ");
    Serial.println(info);
    tft.print("info        ");
    tft.println(info);
}
void audio_id3data(const char *info)
{ // id3 metadata
    Serial.print("id3data     ");
    Serial.println(info);
}
void audio_eof_mp3(const char *info)
{ // end of file
    Serial.print("eof_mp3     ");
    Serial.println(info);
}
void audio_showstation(const char *info)
{
    Serial.print("station     ");
    Serial.println(info);
}
void audio_showstreamtitle(const char *info)
{
    Serial.print("streamtitle ");
    Serial.println(info);
}
void audio_bitrate(const char *info)
{
    Serial.print("bitrate     ");
    Serial.println(info);
}
void audio_commercial(const char *info)
{ // duration in sec
    Serial.print("commercial  ");
    Serial.println(info);
}
void audio_icyurl(const char *info)
{ // homepage
    Serial.print("icyurl      ");
    Serial.println(info);
}
void audio_lasthost(const char *info)
{ // stream URL played
    Serial.print("lasthost    ");
    Serial.println(info);
}
