/*******************************************************************************
 * Touch libraries:
 * FT6X36: https://github.com/strange-v/FT6X36.git
 * GT911: https://github.com/TAMCTec/gt911-arduino.git
 * XPT2046: https://github.com/PaulStoffregen/XPT2046_Touchscreen.git
 ******************************************************************************/

/* uncomment for FT6X36 */
// #define TOUCH_FT6X36
// #define TOUCH_FT6X36_SCL 19
// #define TOUCH_FT6X36_SDA 18
// #define TOUCH_FT6X36_INT 39

/* uncomment for GT911 */
// #define TOUCH_GT911
// #define TOUCH_GT911_SCL 41
// #define TOUCH_GT911_SDA 40
// #define TOUCH_GT911_INT -1
// #define TOUCH_GT911_RST -1
// #define TOUCH_GT911_ROTATION ROTATION_NORMAL

/* uncomment for XPT2046 */
// #define TOUCH_XPT2046
// #define TOUCH_XPT2046_SCK 12
// #define TOUCH_XPT2046_MISO 13
// #define TOUCH_XPT2046_MOSI 11
// #define TOUCH_XPT2046_CS 10
// #define TOUCH_XPT2046_INT 18
// #define TOUCH_XPT2046_ROTATION 0
// #define TOUCH_XPT2046_SAMPLES 50

// Please fill below values from Arduino_GFX Example - TouchCalibration
bool touch_swap_xy = false;
int16_t touch_map_x1 = -1;
int16_t touch_map_x2 = -1;
int16_t touch_map_y1 = -1;
int16_t touch_map_y2 = -1;

int16_t touch_max_x = 0, touch_max_y = 0;
int16_t touch_raw_x = 0, touch_raw_y = 0;
int16_t touch_last_x = 0, touch_last_y = 0;

#if defined(TOUCH_FT6X36)
#include <Wire.h>
#include <FT6X36.h>
FT6X36 ts(&Wire, TOUCH_FT6X36_INT);
bool touch_touched_flag = true, touch_released_flag = true;

#elif defined(TOUCH_GT911)
#include <Wire.h>
#include <TAMC_GT911.h>
TAMC_GT911 ts = TAMC_GT911(TOUCH_GT911_SDA, TOUCH_GT911_SCL, TOUCH_GT911_INT, TOUCH_GT911_RST, max(touch_map_x1, touch_map_x2), max(touch_map_y1, touch_map_y2));

#elif defined(TOUCH_XPT2046)
#include <XPT2046_Touchscreen.h>
#include <SPI.h>
XPT2046_Touchscreen ts(TOUCH_XPT2046_CS, TOUCH_XPT2046_INT);

#endif

#if defined(TOUCH_FT6X36)
void touch(TPoint p, TEvent e)
{
  if (e != TEvent::Tap && e != TEvent::DragStart && e != TEvent::DragMove && e != TEvent::DragEnd)
  {
    return;
  }
  // translation logic depends on screen rotation
  touch_raw_x = p.x;
  touch_raw_y = p.y;
  if (touch_swap_xy)
  {
    touch_last_x = map(touch_raw_y, touch_map_x1, touch_map_x2, 0, touch_max_x);
    touch_last_y = map(touch_raw_x, touch_map_y1, touch_map_y2, 0, touch_max_y);
  }
  else
  {
    touch_last_x = map(touch_raw_x, touch_map_x1, touch_map_x2, 0, touch_max_x);
    touch_last_y = map(touch_raw_y, touch_map_y1, touch_map_y2, 0, touch_max_y);
  }
  switch (e)
  {
  case TEvent::Tap:
    Serial.println("Tap");
    touch_touched_flag = true;
    touch_released_flag = true;
    break;
  case TEvent::DragStart:
    Serial.println("DragStart");
    touch_touched_flag = true;
    break;
  case TEvent::DragMove:
    Serial.println("DragMove");
    touch_touched_flag = true;
    break;
  case TEvent::DragEnd:
    Serial.println("DragEnd");
    touch_released_flag = true;
    break;
  default:
    Serial.println("UNKNOWN");
    break;
  }
}
#endif

void touch_init(int max_x, int max_y)
{
  touch_max_x = max_x;
  touch_max_y = max_y;

#if defined(TOUCH_FT6X36)
  Wire.begin(TOUCH_FT6X36_SDA, TOUCH_FT6X36_SCL);
  ts.begin();
  ts.registerTouchHandler(touch);

#elif defined(TOUCH_GT911)
  Wire.begin(TOUCH_GT911_SDA, TOUCH_GT911_SCL);
  ts.begin();
  ts.setRotation(TOUCH_GT911_ROTATION);

#elif defined(TOUCH_XPT2046)
  SPI.begin(TOUCH_XPT2046_SCK, TOUCH_XPT2046_MISO, TOUCH_XPT2046_MOSI, TOUCH_XPT2046_CS);
  ts.begin();
  ts.setRotation(TOUCH_XPT2046_ROTATION);

#endif
}

bool touch_has_signal()
{
#if defined(TOUCH_FT6X36)
  ts.loop();
  return touch_touched_flag || touch_released_flag;

#elif defined(TOUCH_GT911)
  return true;

#elif defined(TOUCH_XPT2046)
  return ts.tirqTouched();

#else
  return false;
#endif
}

bool touch_touched()
{
#if defined(TOUCH_FT6X36)
  if (touch_touched_flag)
  {
    touch_touched_flag = false;
    return true;
  }

#elif defined(TOUCH_GT911)
  ts.read();
  if (ts.isTouched)
  {
    touch_raw_x = ts.points[0].x;
    touch_raw_y = ts.points[0].y;
    if (touch_swap_xy)
    {
      touch_last_x = map(touch_raw_y, touch_map_x1, touch_map_x2, 0, touch_max_x);
      touch_last_y = map(touch_raw_x, touch_map_y1, touch_map_y2, 0, touch_max_y);
    }
    else
    {
      touch_last_x = map(touch_raw_x, touch_map_x1, touch_map_x2, 0, touch_max_x);
      touch_last_y = map(touch_raw_y, touch_map_y1, touch_map_y2, 0, touch_max_y);
    }
    return true;
  }

#elif defined(TOUCH_XPT2046)
  if (ts.touched())
  {
    TS_Point p = ts.getPoint();
    touch_raw_x = p.x;
    touch_raw_y = p.y;
    int max_z = p.z;
    int count = 0;
    while ((ts.touched()) && (count < TOUCH_XPT2046_SAMPLES))
    {
      count++;

      TS_Point p = ts.getPoint();
      if (p.z > max_z)
      {
        touch_raw_x = p.x;
        touch_raw_y = p.y;
        max_z = p.z;
      }
      // Serial.printf("touch_raw_x: %d, touch_raw_y: %d, p.z: %d\n", touch_raw_x, touch_raw_y, p.z);
    }
    if (touch_swap_xy)
    {
      touch_last_x = map(touch_raw_y, touch_map_x1, touch_map_x2, 0, touch_max_x);
      touch_last_y = map(touch_raw_x, touch_map_y1, touch_map_y2, 0, touch_max_y);
    }
    else
    {
      touch_last_x = map(touch_raw_x, touch_map_x1, touch_map_x2, 0, touch_max_x);
      touch_last_y = map(touch_raw_y, touch_map_y1, touch_map_y2, 0, touch_max_y);
    }
    return true;
  }

#endif
  return false;
}

bool touch_released()
{
#if defined(TOUCH_FT6X36)
  if (touch_released_flag)
  {
    touch_released_flag = false;
    return true;
  }
  else
  {
    return false;
  }

#elif defined(TOUCH_GT911)
  return true;

#elif defined(TOUCH_XPT2046)
  return true;

#else
  return false;
#endif
}
