#ifndef _ARDUINO_NV3023_H_
#define _ARDUINO_NV3023_H_

#include <Arduino.h>
#include <Print.h>
#include "./Arduino_GFX.h"
#include "../Arduino_TFT.h"

#define NV3023_TFTWIDTH 128
#define NV3023_TFTHEIGHT 128

#define NV3023_RST_DELAY 120    ///< delay ms wait for reset finish
#define NV3023_SLPIN_DELAY 120  ///< delay ms wait for sleep in finish
#define NV3023_SLPOUT_DELAY 120 ///< delay ms wait for sleep out finish

#define NV3023_NOP 0x00
#define NV3023_SWRESET 0x01
#define NV3023_RDDID 0x04
#define NV3023_RDDST 0x09

#define NV3023_SLPIN 0x10
#define NV3023_SLPOUT 0x11
#define NV3023_PTLON 0x12
#define NV3023_NORON 0x13

#define NV3023_INVOFF 0x20
#define NV3023_INVON 0x21
#define NV3023_DISPOFF 0x28
#define NV3023_DISPON 0x29

#define NV3023_CASET 0x2A
#define NV3023_RASET 0x2B
#define NV3023_RAMWR 0x2C
#define NV3023_RAMRD 0x2E

#define NV3023_PTLAR 0x30
#define NV3023_TELON 0x35
#define NV3023_MADCTL 0x36
#define NV3023_COLMOD 0x3A
#define NV3023_SCANLSET 0x44

#define NV3023_FRMCTR1 0xB1
#define NV3023_FRMCTR2 0xB2
#define NV3023_FRMCTR3 0xB3

#define NV3023_INVCTR 0xB4
#define NV3023_VREG1CTL 0xE6
#define NV3023_VREG2CTL 0xE7
#define NV3023_GAMMA1 0xF0
#define NV3023_GAMMA2 0xF1
#define NV3023_INTERRE1 0xFE
#define NV3023_INTERRE2 0xEF

#define NV3023_MADCTL_MY 0x80
#define NV3023_MADCTL_MX 0x40
#define NV3023_MADCTL_MV 0x20
#define NV3023_MADCTL_ML 0x10
#define NV3023_MADCTL_BGR 0x08
#define NV3023_MADCTL_MH 0x04
#define NV3023_MADCTL_RGB 0x00

static const uint8_t NV3023_init_operations[] = {
    BEGIN_WRITE,
 
    WRITE_C8_D8, 0xFF, 0xA5,
    WRITE_C8_D8, 0x3E, 0x09,
    WRITE_C8_D8, 0x3A, 0x65,
    WRITE_C8_D8, 0x82, 0x00,
    WRITE_C8_D8, 0x98, 0x00,
    WRITE_C8_D8, 0x63, 0x0F,
    WRITE_C8_D8, 0x64, 0x0F,
    WRITE_C8_D8, 0xB4, 0x34,
    WRITE_C8_D8, 0xB5, 0x30,
    WRITE_C8_D8, 0x83, 0x03,
    WRITE_C8_D8, 0x86, 0x04,
    WRITE_C8_D8, 0x87, 0x16,
    WRITE_C8_D8, 0x88, 0x0A,
    WRITE_C8_D8, 0x89, 0x27,
    WRITE_C8_D8, 0x93, 0x63,
    WRITE_C8_D8, 0x96, 0x81,
    WRITE_C8_D8, 0xC3, 0x10,
    WRITE_C8_D8, 0xE6, 0x00,
    WRITE_C8_D8, 0x99, 0x01, 

////////////////////////gamma_set////////vrp+ v- vrn//////////////////////////////
    WRITE_C8_D8, 0x70, 0x09,
    WRITE_C8_D8, 0x71, 0x1D,
    WRITE_C8_D8, 0x72, 0x14,
    WRITE_C8_D8, 0x73, 0x0A,
    WRITE_C8_D8, 0x74, 0x11,
    WRITE_C8_D8, 0x75, 0x16,
    WRITE_C8_D8, 0x76, 0x38,
    WRITE_C8_D8, 0x77, 0x0B,
    WRITE_C8_D8, 0x78, 0x08,
    WRITE_C8_D8, 0x79, 0x3E,
    WRITE_C8_D8, 0x7A, 0x07,
    WRITE_C8_D8, 0x7B, 0x0D,
    WRITE_C8_D8, 0x7C, 0x16,
    WRITE_C8_D8, 0x7D, 0x0F,
    WRITE_C8_D8, 0x7E, 0x14,
    WRITE_C8_D8, 0x7F, 0x05,
    WRITE_C8_D8, 0xA0, 0x04,
    WRITE_C8_D8, 0xA1, 0x28,
    WRITE_C8_D8, 0xA2, 0x0C,
    WRITE_C8_D8, 0xA3, 0x11,
    WRITE_C8_D8, 0xA4, 0x0B,
    WRITE_C8_D8, 0xA5, 0x23,
    WRITE_C8_D8, 0xA6, 0x45,
    WRITE_C8_D8, 0xA7, 0x07,
    WRITE_C8_D8, 0xA8, 0x0A,
    WRITE_C8_D8, 0xA9, 0x3B,
    WRITE_C8_D8, 0xAA, 0x0D,
    WRITE_C8_D8, 0xAB, 0x18,
    WRITE_C8_D8, 0xAC, 0x14,
    WRITE_C8_D8, 0xAD, 0x0F,
    WRITE_C8_D8, 0xAE, 0x19,
    WRITE_C8_D8, 0xAF, 0x08,

//////////////////////////////////////////////////////////////////
 
    WRITE_C8_D8, 0xFF, 0x00,

    WRITE_COMMAND_8, 0x11,
    END_WRITE,

    DELAY, 120,

    BEGIN_WRITE,
    // WRITE_COMMAND_8, 0x21,
    // WRITE_C8_D8, 0x36, 0x80,
    WRITE_COMMAND_8, 0x29,
    END_WRITE,

    DELAY, 20};

class Arduino_NV3023 : public Arduino_TFT
{
public:
  Arduino_NV3023(
      Arduino_DataBus *bus, int8_t rst = GFX_NOT_DEFINED, uint8_t r = 0,
      bool ips = false, int16_t w = NV3023_TFTWIDTH, int16_t h = NV3023_TFTHEIGHT,
      uint8_t col_offset1 = 0, uint8_t row_offset1 = 0, uint8_t col_offset2 = 0, uint8_t row_offset2 = 0);

  bool begin(int32_t speed = GFX_NOT_DEFINED) override;
  void writeAddrWindow(int16_t x, int16_t y, uint16_t w, uint16_t h) override;
  void setRotation(uint8_t r) override;
  void invertDisplay(bool) override;
  void displayOn() override;
  void displayOff() override;

protected:
  void tftInit() override;

private:
};

#endif
