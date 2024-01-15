#include "../Arduino_DataBus.h"

#if (ESP_ARDUINO_VERSION_MAJOR < 3)

#if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S3)

#include "../Arduino_GFX.h"
#include "Arduino_RGB_Display.h"

Arduino_RGB_Display::Arduino_RGB_Display(
    int16_t w, int16_t h, Arduino_ESP32RGBPanel *rgbpanel, uint8_t r, bool auto_flush,
    Arduino_DataBus *bus, int8_t rst, const uint8_t *init_operations, size_t init_operations_len,
    uint8_t col_offset1, uint8_t row_offset1, uint8_t col_offset2, uint8_t row_offset2)
    : Arduino_GFX(w, h), _rgbpanel(rgbpanel), _auto_flush(auto_flush),
      _bus(bus), _rst(rst), _init_operations(init_operations), _init_operations_len(init_operations_len),
      COL_OFFSET1(col_offset1), ROW_OFFSET1(row_offset1),
      COL_OFFSET2(col_offset2), ROW_OFFSET2(row_offset2)
{
  _fb_width = COL_OFFSET1 + WIDTH + COL_OFFSET2;
  _fb_height = ROW_OFFSET1 + HEIGHT + ROW_OFFSET2;
  _fb_max_x = _fb_width - 1;
  _fb_max_y = _fb_height - 1;
  _framebuffer_size = _fb_width * _fb_height * 2;
  MAX_X = WIDTH - 1;
  MAX_Y = HEIGHT - 1;
  setRotation(r);
}

bool Arduino_RGB_Display::begin(int32_t speed)
{
  if (_bus)
  {
    if (!_bus->begin())
    {
      return false;
    }
  }

  if (_rst != GFX_NOT_DEFINED)
  {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(100);
    digitalWrite(_rst, LOW);
    delay(120);
    digitalWrite(_rst, HIGH);
    delay(120);
  }
  else
  {
    if (_bus)
    {
      // Software Rest
      _bus->sendCommand(0x01);
      delay(120);
    }
  }

  if (_bus)
  {
    if (_init_operations_len > 0)
    {
      _bus->batchOperation((uint8_t *)_init_operations, _init_operations_len);
    }
  }

  _rgbpanel->begin(speed);
  _framebuffer = _rgbpanel->getFrameBuffer(_fb_width, _fb_height);

  if (!_framebuffer)
  {
    return false;
  }

  return true;
}

void Arduino_RGB_Display::writePixelPreclipped(int16_t x, int16_t y, uint16_t color)
{
  x += _xStart;
  y += _yStart;
  uint16_t *fb = _framebuffer;
  switch (_rotation)
  {
  case 1:
    fb += (int32_t)x * _fb_width;
    fb += _fb_max_x - y;
    *fb = color;
    if (_auto_flush)
    {
      Cache_WriteBack_Addr((uint32_t)fb, 2);
    }
    break;
  case 2:
    fb += (int32_t)(_fb_max_y - y) * _fb_width;
    fb += _fb_max_x - x;
    *fb = color;
    if (_auto_flush)
    {
      Cache_WriteBack_Addr((uint32_t)fb, 2);
    }
    break;
  case 3:
    fb += (int32_t)(_fb_max_y - x) * _fb_width;
    fb += y;
    *fb = color;
    if (_auto_flush)
    {
      Cache_WriteBack_Addr((uint32_t)fb, 2);
    }
    break;
  default: // case 0:
    fb += (int32_t)y * _fb_width;
    fb += x;
    *fb = color;
    if (_auto_flush)
    {
      Cache_WriteBack_Addr((uint32_t)fb, 2);
    }
  }
}

void Arduino_RGB_Display::writeFastVLine(int16_t x, int16_t y,
                                         int16_t h, uint16_t color)
{
  // log_i("writeFastVLine(x: %d, y: %d, h: %d)", x, y, h);
  switch (_rotation)
  {
  case 1:
    writeFastHLineCore(_height - y - h, x, h, color);
    break;
  case 2:
    writeFastVLineCore(_max_x - x, _height - y - h, h, color);
    break;
  case 3:
    writeFastHLineCore(y, _max_x - x, h, color);
    break;
  default: // case 0:
    writeFastVLineCore(x, y, h, color);
  }
}

void Arduino_RGB_Display::writeFastVLineCore(int16_t x, int16_t y,
                                             int16_t h, uint16_t color)
{
  // log_i("writeFastVLineCore(x: %d, y: %d, h: %d)", x, y, h);
  if (_ordered_in_range(x, 0, MAX_X) && h)
  { // X on screen, nonzero height
    if (h < 0)
    {             // If negative height...
      y += h + 1; //   Move Y to top edge
      h = -h;     //   Use positive height
    }
    if (y <= MAX_Y)
    { // Not off bottom
      int16_t y2 = y + h - 1;
      if (y2 >= 0)
      { // Not off top
        // Line partly or fully overlaps screen
        if (y < 0)
        {
          y = 0;
          h = y2 + 1;
        } // Clip top
        if (y2 > MAX_Y)
        {
          h = MAX_Y - y + 1;
        } // Clip bottom

        x += COL_OFFSET1;
        y += ROW_OFFSET1;
        uint16_t *fb = _framebuffer + ((int32_t)y * _fb_width) + x;
        if (_auto_flush)
        {
          while (h--)
          {
            *fb = color;
            Cache_WriteBack_Addr((uint32_t)fb, 2);
            fb += _fb_width;
          }
        }
        else
        {
          while (h--)
          {
            *fb = color;
            fb += _fb_width;
          }
        }
      }
    }
  }
}

void Arduino_RGB_Display::writeFastHLine(int16_t x, int16_t y,
                                         int16_t w, uint16_t color)
{
  // log_i("writeFastHLine(x: %d, y: %d, w: %d)", x, y, w);
  switch (_rotation)
  {
  case 1:
    writeFastVLineCore(_max_y - y, x, w, color);
    break;
  case 2:
    writeFastHLineCore(_width - x - w, _max_y - y, w, color);
    break;
  case 3:
    writeFastVLineCore(y, _width - x - w, w, color);
    break;
  default: // case 0:
    writeFastHLineCore(x, y, w, color);
  }
}

void Arduino_RGB_Display::writeFastHLineCore(int16_t x, int16_t y,
                                             int16_t w, uint16_t color)
{
  // log_i("writeFastHLineCore(x: %d, y: %d, w: %d)", x, y, w);
  if (_ordered_in_range(y, 0, MAX_Y) && w)
  { // Y on screen, nonzero width
    if (w < 0)
    {             // If negative width...
      x += w + 1; //   Move X to left edge
      w = -w;     //   Use positive width
    }
    if (x <= MAX_X)
    { // Not off right
      int16_t x2 = x + w - 1;
      if (x2 >= 0)
      { // Not off left
        // Line partly or fully overlaps screen
        if (x < 0)
        {
          x = 0;
          w = x2 + 1;
        } // Clip left
        if (x2 > MAX_X)
        {
          w = MAX_X - x + 1;
        } // Clip right

        x += COL_OFFSET1;
        y += ROW_OFFSET1;
        uint16_t *fb = _framebuffer + ((int32_t)y * _fb_width) + x;
        uint32_t cachePos = (uint32_t)fb;
        int16_t writeSize = w * 2;
        while (w--)
        {
          *(fb++) = color;
        }
        if (_auto_flush)
        {
          Cache_WriteBack_Addr(cachePos, writeSize);
        }
      }
    }
  }
}

void Arduino_RGB_Display::writeFillRectPreclipped(int16_t x, int16_t y,
                                                  int16_t w, int16_t h, uint16_t color)
{
  // log_i("writeFillRectPreclipped(x: %d, y: %d, w: %d, h: %d)", x, y, w, h);
  if (_rotation > 0)
  {
    int16_t t = x;
    switch (_rotation)
    {
    case 1:
      x = WIDTH - y - h;
      y = t;
      t = w;
      w = h;
      h = t;
      break;
    case 2:
      x = WIDTH - x - w;
      y = HEIGHT - y - h;
      break;
    case 3:
      x = y;
      y = HEIGHT - t - w;
      t = w;
      w = h;
      h = t;
      break;
    }
  }
  // log_i("adjusted writeFillRectPreclipped(x: %d, y: %d, w: %d, h: %d)", x, y, w, h);
  x += COL_OFFSET1;
  y += ROW_OFFSET1;
  uint16_t *row = _framebuffer;
  row += y * _fb_width;
  uint32_t cachePos = (uint32_t)row;
  row += x;
  for (int j = 0; j < h; j++)
  {
    for (int i = 0; i < w; i++)
    {
      row[i] = color;
    }
    row += _fb_width;
  }
  if (_auto_flush)
  {
    Cache_WriteBack_Addr(cachePos, _fb_width * h * 2);
  }
}

void Arduino_RGB_Display::drawIndexedBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else
  {
    if (_rotation > 0)
    {
      Arduino_GFX::drawIndexedBitmap(x, y, bitmap, color_index, w, h, x_skip);
    }
    else
    {
      if ((y + h - 1) > _max_y)
      {
        h -= (y + h - 1) - _max_y;
      }
      if (y < 0)
      {
        bitmap -= y * (w + x_skip);
        h += y;
        y = 0;
      }
      if ((x + w - 1) > _max_x)
      {
        x_skip += (x + w - 1) - _max_x;
        w -= (x + w - 1) - _max_x;
      }
      if (x < 0)
      {
        bitmap -= x;
        x_skip -= x;
        w += x;
        x = 0;
      }

      x += COL_OFFSET1;
      y += ROW_OFFSET1;
      uint16_t *row = _framebuffer;
      row += y * _fb_width;
      uint32_t cachePos = (uint32_t)row;
      row += x;
      for (int j = 0; j < h; j++)
      {
        for (int i = 0; i < w; i++)
        {
          row[i] = color_index[*bitmap++];
        }
        bitmap += x_skip;
        row += _fb_width;
      }
      if (_auto_flush)
      {
        Cache_WriteBack_Addr(cachePos, _fb_width * h * 2);
      }
    }
  }
}

void Arduino_RGB_Display::draw16bitRGBBitmap(int16_t x, int16_t y,
                                             uint16_t *bitmap, int16_t w, int16_t h)
{
  if (_isRoundMode)
  {
    if (
        ((y + h - 1) < 0) || // Outside top
        (y > _max_y) ||      // Outside bottom
        (
            (x > _roundMaxX[y + h - 1]) &&        // top left
            ((x + w - 1) < _roundMinX[y]) &&      // top right
            (x > _roundMaxX[y + h - 1]) &&        // bottom left
            ((x + w - 1) < _roundMinX[y + h - 1]) // bottom right
            ))
    {
      return;
    }
  }

  bool result;

  x += COL_OFFSET1;
  y += ROW_OFFSET1;
  switch (_rotation)
  {
  case 1:
    result = gfx_draw_bitmap_to_framebuffer_rotate_1(bitmap, w, h, _framebuffer, x, y, _fb_width, _height + ROW_OFFSET1);
    break;
  case 2:
    result = gfx_draw_bitmap_to_framebuffer_rotate_2(bitmap, w, h, _framebuffer, x, y, _fb_width, _height + ROW_OFFSET1);
    break;
  case 3:
    result = gfx_draw_bitmap_to_framebuffer_rotate_3(bitmap, w, h, _framebuffer, x, y, _fb_width, _height + ROW_OFFSET1);
    break;
  default: // case 0:
    result = gfx_draw_bitmap_to_framebuffer(bitmap, w, h, _framebuffer, x, y, _fb_width, _height + ROW_OFFSET1);
  }

  if (result)
  {
    if (_auto_flush)
    {
      uint32_t cachePos;
      size_t cache_size;
      switch (_rotation)
      {
      case 1:
        cachePos = (uint32_t)(_framebuffer + (x * _fb_width));
        cache_size = _fb_width * w * 2;
        break;
      case 2:
        cachePos = (uint32_t)(_framebuffer + ((HEIGHT - y - h) * _fb_width));
        cache_size = _fb_width * h * 2;
        break;
      case 3:
        cachePos = (uint32_t)(_framebuffer + ((HEIGHT - x - w) * _fb_width));
        cache_size = _fb_width * w * 2;
        break;
      default: // case 0:
        cachePos = (uint32_t)(_framebuffer + (y * _fb_width) + x);
        cache_size = (_fb_width * (h - 1) + w) * 2;
      }
      Cache_WriteBack_Addr(cachePos, cache_size);
    }
  }
}

void Arduino_RGB_Display::draw16bitBeRGBBitmap(int16_t x, int16_t y,
                                               uint16_t *bitmap, int16_t w, int16_t h)
{
  if (
      ((x + w - 1) < 0) || // Outside left
      ((y + h - 1) < 0) || // Outside top
      (x > _max_x) ||      // Outside right
      (y > _max_y)         // Outside bottom
  )
  {
    return;
  }
  else
  {
    if (_rotation > 0)
    {
      Arduino_GFX::draw16bitBeRGBBitmap(x, y, bitmap, w, h);
    }
    else
    {
      x += COL_OFFSET1;
      y += ROW_OFFSET1;
      int16_t x_skip = 0;
      if ((y + h - 1) > _max_y)
      {
        h -= (y + h - 1) - _max_y;
      }
      if (y < 0)
      {
        bitmap -= y * w;
        h += y;
        y = 0;
      }
      if ((x + w - 1) > _max_x)
      {
        x_skip = (x + w - 1) - _max_x;
        w -= x_skip;
      }
      if (x < 0)
      {
        bitmap -= x;
        x_skip -= x;
        w += x;
        x = 0;
      }
      uint16_t *row = _framebuffer;
      row += y * _fb_width;
      uint32_t cachePos = (uint32_t)row;
      row += x;
      uint16_t color;
      for (int j = 0; j < h; j++)
      {
        for (int i = 0; i < w; i++)
        {
          color = *bitmap++;
          MSB_16_SET(row[i], color);
        }
        bitmap += x_skip;
        row += _fb_width;
      }
      if (_auto_flush)
      {
        Cache_WriteBack_Addr(cachePos, _fb_width * h * 2);
      }
    }
  }
}

void Arduino_RGB_Display::flush(void)
{
  if (!_auto_flush)
  {
    Cache_WriteBack_Addr((uint32_t)_framebuffer, _framebuffer_size);
  }
}

uint16_t *Arduino_RGB_Display::getFramebuffer()
{
  return _framebuffer;
}

#endif // #if defined(ESP32) && (CONFIG_IDF_TARGET_ESP32S3)

#endif // #if (ESP_ARDUINO_VERSION_MAJOR < 3)
