# Adafruit MPR121 Library [![Build Status](https://travis-ci.org/adafruit/Adafruit_MPR121.svg?branch=master)](https://travis-ci.org/adafruit/Adafruit_MPR121)

<a href="https://www.adafruit.com/products/1982"><img src="https://cdn-shop.adafruit.com/970x728/1982-00.jpg" height="300"/></a>

Tested and works great with the Adafruit MPR121
  * https://www.adafruit.com/products/1982
  * https://www.adafruit.com/product/2024
  * https://www.adafruit.com/product/2340
 
Check out the links above for our tutorials and wiring diagrams. 
These sensors use I2C to communicate, 2+ pins are required to interface

Adafruit invests time and resources providing this open source code, please support Adafruit and open-source hardware by purchasing products from Adafruit!

Written by Limor Fried/Ladyada (Adafruit Industries).
MIT license, all text above must be included in any redistribution

<!-- START COMPATIBILITY TABLE -->

## Compatibility

MCU                | Tested Works | Doesn't Work | Not Tested  | Notes
------------------ | :----------: | :----------: | :---------: | -----
Atmega328 @ 16MHz  |      X       |             |            | 
Atmega328 @ 12MHz  |      X       |             |            | 
Atmega32u4 @ 16MHz |      X       |             |            | 
Atmega32u4 @ 8MHz  |      X       |             |            | 
ESP8266            |      X       |             |            | 
Atmega2560 @ 16MHz |      X       |             |            | 
ATSAM3X8E          |             |      X       |            | 
ATSAM21D           |      X       |             |            | 
ATtiny85 @ 16MHz   |             |             |     X       | 
ATtiny85 @ 8MHz    |             |             |     X       | 
Intel Curie @ 32MHz |             |             |     X       | 
STM32F2            |             |             |     X       | 

  * ATmega328 @ 16MHz : Arduino UNO, Adafruit Pro Trinket 5V, Adafruit Metro 328, Adafruit Metro Mini
  * ATmega328 @ 12MHz : Adafruit Pro Trinket 3V
  * ATmega32u4 @ 16MHz : Arduino Leonardo, Arduino Micro, Arduino Yun, Teensy 2.0
  * ATmega32u4 @ 8MHz : Adafruit Flora, Bluefruit Micro
  * ESP8266 : Adafruit Huzzah
  * ATmega2560 @ 16MHz : Arduino Mega
  * ATSAM3X8E : Arduino Due
  * ATSAM21D : Arduino Zero, M0 Pro
  * ATtiny85 @ 16MHz : Adafruit Trinket 5V
  * ATtiny85 @ 8MHz : Adafruit Gemma, Arduino Gemma, Adafruit Trinket 3V

<!-- END COMPATIBILITY TABLE -->
