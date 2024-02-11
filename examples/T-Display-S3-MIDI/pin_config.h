/*
 * @Description: None
 * @version: None
 * @Author: None
 * @Date: 2023-06-05 13:01:59
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-07-26 16:14:43
 */
#pragma once

/*ESP32S3*/
#define PIN_LCD_BL 38

#define PIN_LCD_D0 39
#define PIN_LCD_D1 40
#define PIN_LCD_D2 41
#define PIN_LCD_D3 42
#define PIN_LCD_D4 45
#define PIN_LCD_D5 46
#define PIN_LCD_D6 47
#define PIN_LCD_D7 48

#define PIN_POWER_ON 15

#define PIN_LCD_RES 5
#define PIN_LCD_CS 6
#define PIN_LCD_DC 7
#define PIN_LCD_WR 8
#define PIN_LCD_RD 9

#define PIN_BUTTON_1 0
#define PIN_BUTTON_2 14
#define PIN_BAT_VOLT 4

#define PIN_IIC_SCL 17
#define PIN_IIC_SDA 18

#define PIN_TOUCH_INT 16
#define PIN_TOUCH_RES 21

/* External expansion */
// SD
#define PIN_SD_CS 16
#define PIN_SD_CLK 21
#define PIN_SD_MISO 3
#define PIN_SD_MOSI 10

// MPR121_PIN
#define MPR121_ADDR1 0x5A
#define MPR121_ADDR2 0x5B // VCC
#define MPR121_IRQ_PIN 2
#define MPR121_SDA 18
#define MPR121_SCL 17

// PCF8575_PIN
#define T_DISPLAY_PCF8575_IIC_ADDRESS 0x20
#define PCF8575_IRQ_PIN 2
#define PCF8575_SDA 18
#define PCF8575_SCL 17
#define T_DISPLAY_PCF8575_INTERRUPTPIN 2

// PCM5102A_PIN
#define PCM5102A_BCLK 11
#define PCM5102A_LRCLK 13
#define PCM5102A_DIN 12

// XL95x5_PIN
#define T_DISPLAY_XL95x5_IIC_ADDRESS 0X20
#define T_DISPLAY_XL95x5_SDA 18 // Default configuration
#define T_DISPLAY_XL95x5_SCL 17
#define T_DISPLAY_XL95x5_INTERRUPTPIN 2

// N085-1212TBWIG06-C08
#define N085_CS1 13
#define N085_CS2 12
#define N085_CS3 14
#define N085_CS4 21
#define N085_DC 9
#define N085_SCK 11
#define N085_MOSI 10
#define N085_MISO -1
#define N085_RST 46
#define N085_BL 3

enum
{
    N085_Initialize = 0,
    N085_Screen_1,
    N085_Screen_2,
    N085_Screen_3,
    N085_Screen_4,
    N085_Screen_ALL,
};
