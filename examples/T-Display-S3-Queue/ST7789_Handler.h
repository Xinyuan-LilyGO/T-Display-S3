#ifndef ST7789_HANDLER_H
#define ST7789_HANDLER_H

#include <LovyanGFX.h>
#include <vector>
#include <string>

// ST7789_Handler class:
// Manages the ST7789 display on the T-Display-S3.
class ST7789_Handler {
public:
  ST7789_Handler();

  // Initializes the display (rotation, font, color, etc.)
  void init();

  // Displays a formatted string on screen, avoiding flickering.
  void print(const std::string& message);

  // Displays a vector of strings concatenated on screen, avoiding flickering.
  void print(const std::vector<std::string>& messages);

  // Displays a formatted string on screen with line breaks, avoiding flickering.
  void println(const std::string& message);

  // Displays a vector of strings with line breaks, avoiding flickering.
  void println(const std::vector<std::string>& messages);

  // Clears the display
  void clear();

private:
  // Custom class based on LovyanGFX to configure the ST7789 panel via 8-bit parallel bus
  class LGFX : public lgfx::LGFX_Device {
  public:
    LGFX(void) {
      {  // 8-bit parallel bus configuration
        auto cfg = _bus_instance.config();
        cfg.pin_wr = 8;
        cfg.pin_rd = 9;
        cfg.pin_rs = 7;  // D/C
        cfg.pin_d0 = 39;
        cfg.pin_d1 = 40;
        cfg.pin_d2 = 41;
        cfg.pin_d3 = 42;
        cfg.pin_d4 = 45;
        cfg.pin_d5 = 46;
        cfg.pin_d6 = 47;
        cfg.pin_d7 = 48;
        _bus_instance.config(cfg);
        _panel_instance.setBus(&_bus_instance);
      }
      {  // ST7789 panel configuration
        auto cfg = _panel_instance.config();
        cfg.pin_cs = 6;
        cfg.pin_rst = 5;
        cfg.pin_busy = -1;
        cfg.offset_rotation = 1;
        cfg.offset_x = 35;
        cfg.readable = false;
        cfg.invert = true;
        cfg.rgb_order = false;
        cfg.dlen_16bit = false;
        cfg.bus_shared = false;
        // Landscape orientation: 170 x 320 display
        cfg.panel_width = 170;
        cfg.panel_height = 320;
        _panel_instance.config(cfg);
      }
      setPanel(&_panel_instance);
      {  // Backlight configuration
        auto cfg = _light_instance.config();
        cfg.pin_bl = 38;
        cfg.invert = false;
        cfg.freq = 22000;
        cfg.pwm_channel = 7;
        _light_instance.config(cfg);
        _panel_instance.setLight(&_light_instance);
      }
    }
  private:
    lgfx::Bus_Parallel8 _bus_instance;
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Light_PWM _light_instance;
  };

  LGFX tft;
};

extern ST7789_Handler display;

#endif  // ST7789_HANDLER_H
