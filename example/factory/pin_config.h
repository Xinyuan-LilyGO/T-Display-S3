#pragma once

// #define WIFI_SSID                    "Your-ssid"
// #define WIFI_PASSWORD                "Your-password"

#define WIFI_SSID                    "Your-ssid"
#define WIFI_PASSWORD                "Your-password"

#define WIFI_CONNECT_WAIT_MAX        (30 * 1000)

#define NTP_SERVER1                  "pool.ntp.org"
#define NTP_SERVER2                  "time.nist.gov"
#define GMT_OFFSET_SEC               0
#define DAY_LIGHT_OFFSET_SEC         0
// if CUSTOM_TIMEZONE is not defined then TIMEZONE API used based on IP, check zones.h
// #define CUSTOM_TIMEZONE             "Europe/London"

/* Automatically update local time */
#define GET_TIMEZONE_API             "https://ipapi.co/timezone/"

/* LCD CONFIG */
// Too low or too high pixel clock may cause screen mosaic
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ   (16 * 1000 * 1000)
// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES            320
#define EXAMPLE_LCD_V_RES            170
#define LVGL_LCD_BUF_SIZE            (EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES)
#define EXAMPLE_PSRAM_DATA_ALIGNMENT 64

/*ESP32S3*/
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

#define PIN_IIC_SCL                  17
#define PIN_IIC_SDA                  18

#define PIN_TOUCH_INT                16
#define PIN_TOUCH_RES                21

/* External expansion */
#define PIN_SD_CMD                   13
#define PIN_SD_CLK                   11
#define PIN_SD_D0                    12
