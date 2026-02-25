#ifndef MAPPING_H
#define MAPPING_H

// Pin mapping for T-Display S3

// USB Host
#define USB_DP_PIN   19
#define USB_DN_PIN   18

// ST7789 Display
#define TFT_CS_PIN    6
#define TFT_DC_PIN    16
#define TFT_RST_PIN   5
#define TFT_BL_PIN    38

// I2S Audio (PCM5102A)
#define I2S_BCK_PIN       11
#define I2S_WS_PIN        13
#define I2S_DATA_OUT_PIN  12

// Board hardware
#define PIN_POWER_ON 15
#define PIN_BUTTON_1  0
#define PIN_BUTTON_2 14
#define PIN_BAT_VOLT  4

// I2C (alternate pins — required when USB Host is active on pins 18/19)
#define PIN_IIC_SCL 43
#define PIN_IIC_SDA 44

#endif // MAPPING_H
