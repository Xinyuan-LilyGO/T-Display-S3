/*
 * PCF8575 GPIO Port Expand
 * https://www.mischianti.org/2019/07/22/pcf8575-i2c-16-bit-digital-i-o-expander/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Renzo Mischianti www.mischianti.org All right reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef PCF8575_h
#define PCF8575_h

#include "Wire.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define DEFAULT_SDA SDA;
#define DEFAULT_SCL SCL;

// Uncomment to enable printing out nice debug messages.
// #define PCF8575_DEBUG

// Uncomment for low memory usage this prevent use of complex DigitalInput structure and free 7byte of memory
// #define PCF8575_LOW_MEMORY

// Define where debug output will be printed.
#define DEBUG_PRINTER Serial

// Define to manage original pinout of pcf8575
// like datasheet but not sequential
//#define NOT_SEQUENTIAL_PINOUT

// Setup debug printing macros.
#ifdef PCF8575_DEBUG
	#define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
	#define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
#else
	#define DEBUG_PRINT(...) {}
	#define DEBUG_PRINTLN(...) {}
#endif

#define READ_ELAPSED_TIME 10

//#define P0  	B00000001
//#define P1  	B00000010
//#define P2  	B00000100
//#define P3  	B00001000
//#define P4  	B00010000
//#define P5  	B00100000
//#define P6  	B01000000
//#define P7  	B10000000
//
#ifdef NOT_SEQUENTIAL_PINOUT
	#define P00  	0
	#define P01  	1
	#define P02  	2
	#define P03  	3
	#define P04  	4
	#define P05  	5
	#define P06  	6
	#define P07  	7
	#define P10  	8
	#define P11  	9
	#define P12  	10
	#define P13  	11
	#define P14  	12
	#define P15  	13
	#define P16  	14
	#define P17  	15
#else
	#define P0  	0
	#define P1  	1
	#define P2  	2
	#define P3  	3
	#define P4  	4
	#define P5  	5
	#define P6  	6
	#define P7  	7
	#define P8  	8
	#define P9  	9
	#define P10  	10
	#define P11  	11
	#define P12  	12
	#define P13  	13
	#define P14  	14
	#define P15  	15
#endif

#include <math.h>


class PCF8575 {
public:

	PCF8575(uint8_t address);
	PCF8575(uint8_t address, uint8_t interruptPin,  void (*interruptFunction)() );

#if !defined(__AVR) && !defined(ARDUINO_ARCH_SAMD) && !defined(TEENSYDUINO)
	PCF8575(uint8_t address, int sda, int scl);
	PCF8575(uint8_t address, int sda, int scl, uint8_t interruptPin,  void (*interruptFunction)());
#endif

#if defined(ESP32) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_STM32)
	///// changes for second i2c bus
	PCF8575(TwoWire *pWire, uint8_t address);
	PCF8575(TwoWire *pWire, uint8_t address, uint8_t interruptPin,  void (*interruptFunction)() );
#endif
#if defined(ESP32)
	PCF8575(TwoWire *pWire, uint8_t address, int sda, int scl);
	PCF8575(TwoWire *pWire, uint8_t address, int sda, int scl, uint8_t interruptPin,  void (*interruptFunction)());
#endif

	void begin();
	void pinMode(uint8_t pin, uint8_t mode);

	void readBuffer(bool force = true);
	uint8_t digitalRead(uint8_t pin);
	#ifndef PCF8575_LOW_MEMORY
		struct DigitalInput {
#ifdef NOT_SEQUENTIAL_PINOUT
		uint8_t p00;
		uint8_t p01;
		uint8_t p02;
		uint8_t p03;
		uint8_t p04;
		uint8_t p05;
		uint8_t p06;
		uint8_t p07;
		uint8_t p10;
		uint8_t p11;
		uint8_t p12;
		uint8_t p13;
		uint8_t p14;
		uint8_t p15;
		uint8_t p16;
		uint8_t p17;
#else
		uint8_t p0;
		uint8_t p1;
		uint8_t p2;
		uint8_t p3;
		uint8_t p4;
		uint8_t p5;
		uint8_t p6;
		uint8_t p7;
		uint8_t p8;
		uint8_t p9;
		uint8_t p10;
		uint8_t p11;
		uint8_t p12;
		uint8_t p13;
		uint8_t p14;
		uint8_t p15;
#endif
		} digitalInput;


		DigitalInput digitalReadAll(void);
	#else
		uint16_t digitalReadAll(void);
	#endif
	void digitalWrite(uint8_t pin, uint8_t value);

private:
	uint8_t _address;

	#if !defined(DEFAULT_SDA)
	#  if defined(ARDUINO_ARCH_STM32)
	#    define DEFAULT_SDA PB7
	#  elif defined(ESP8266)
	#    define DEFAULT_SDA 4
	#  elif defined(SDA)
	#    define DEFAULT_SDA SDA
	#  else
	#    error "Error define DEFAULT_SDA, SDA not declared, if you have this error contact the mantainer"
	#  endif
	#endif
	#if !defined(DEFAULT_SCL)
	#  if defined(ARDUINO_ARCH_STM32)
	#    define DEFAULT_SCL PB6
	#  elif defined(ESP8266)
	#    define DEFAULT_SCL 5
	#  elif defined(SDA)
	#    define DEFAULT_SCL SCL
	#  else
	#    error "Error define DEFAULT_SCL, SCL not declared, if you have this error contact the mantainer"
	#  endif
	#endif

	int _sda = DEFAULT_SDA;
	int _scl = DEFAULT_SCL;

	TwoWire *_wire;

	bool _usingInterrupt = false;
	uint8_t _interruptPin = 2;
	void (*_interruptFunction)(){};

	uint16_t writeMode 	= 	0;
	uint16_t readMode 	= 	0;
	uint16_t byteBuffered = 0;
	unsigned long lastReadMillis = 0;

	uint16_t writeByteBuffered = 0;

};

#endif
