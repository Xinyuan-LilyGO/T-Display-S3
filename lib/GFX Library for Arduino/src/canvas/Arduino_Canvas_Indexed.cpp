#include "../Arduino_DataBus.h"
#if !defined(LITTLE_FOOT_PRINT)

#include "../Arduino_GFX.h"
#include "Arduino_Canvas_Indexed.h"

Arduino_Canvas_Indexed::Arduino_Canvas_Indexed(int16_t w, int16_t h, Arduino_G *output, int16_t output_x, int16_t output_y, uint8_t r, uint8_t mask_level)
    : Arduino_GFX(w, h), _output(output), _output_x(output_x), _output_y(output_y)
{
  MAX_X = WIDTH - 1;
  MAX_Y = HEIGHT - 1;
  setRotation(r);

  if (mask_level >= MAXMASKLEVEL)
  {
    mask_level = MAXMASKLEVEL - 1;
  }
  _current_mask_level = mask_level;
  _color_mask = mask_level_list[_current_mask_level];
}

Arduino_Canvas_Indexed::~Arduino_Canvas_Indexed()
{
  if (_framebuffer)
  {
    free(_framebuffer);
  }
}

bool Arduino_Canvas_Indexed::begin(int32_t speed)
{
  if (speed != GFX_SKIP_OUTPUT_BEGIN)
  {
    if (!_output->begin(speed))
    {
      return false;
    }
  }

  if (!_framebuffer)
  {
    size_t s = _width * _height;
#if defined(ESP32)
    if (psramFound())
    {
      _framebuffer = (uint8_t *)ps_malloc(s);
    }
    else
    {
      _framebuffer = (uint8_t *)malloc(s);
    }
#else
    _framebuffer = (uint8_t *)malloc(s);
#endif
    if (!_framebuffer)
    {
      return false;
    }
  }

  return true;
}

void Arduino_Canvas_Indexed::writePixelPreclipped(int16_t x, int16_t y, uint16_t color)
{
  uint8_t idx;
  if (_isDirectUseColorIndex)
  {
    idx = (uint8_t)color;
  }
  else
  {
    idx = get_color_index(color);
  }

  uint8_t *fb = _framebuffer;
  switch (_rotation)
  {
  case 1:
    fb += (int32_t)x * _height;
    fb += _max_y - y;
    *fb = idx;
    break;
  case 2:
    fb += (int32_t)(_max_y - y) * _width;
    fb += _max_x - x;
    *fb = idx;
    break;
  case 3:
    fb += (int32_t)(_max_x - x) * _height;
    fb += y;
    *fb = idx;
    break;
  default: // case 0:
    fb += (int32_t)y * _width;
    fb += x;
    *fb = idx;
  }
}

void Arduino_Canvas_Indexed::writeFastVLine(int16_t x, int16_t y,
                                            int16_t h, uint16_t color)
{
  uint8_t idx;
  if (_isDirectUseColorIndex)
  {
    idx = (uint8_t)color;
  }
  else
  {
    idx = get_color_index(color);
  }

  switch (_rotation)
  {
  case 1:
    writeFastHLineCore(_height - y - h, x, h, idx);
    break;
  case 2:
    writeFastVLineCore(_max_x - x, _height - y - h, h, idx);
    break;
  case 3:
    writeFastHLineCore(y, _max_x - x, h, idx);
    break;
  default: // case 0:
    writeFastVLineCore(x, y, h, idx);
  }
}

void Arduino_Canvas_Indexed::writeFastVLineCore(int16_t x, int16_t y,
                                                int16_t h, uint8_t idx)
{
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

        uint8_t *fb = _framebuffer + ((int32_t)y * WIDTH) + x;
        while (h--)
        {
          *fb = idx;
          fb += WIDTH;
        }
      }
    }
  }
}

void Arduino_Canvas_Indexed::writeFastHLine(int16_t x, int16_t y,
                                            int16_t w, uint16_t color)
{
  uint8_t idx;
  if (_isDirectUseColorIndex)
  {
    idx = (uint8_t)color;
  }
  else
  {
    idx = get_color_index(color);
  }

  switch (_rotation)
  {
  case 1:
    writeFastVLineCore(_max_y - y, x, w, idx);
    break;
  case 2:
    writeFastHLineCore(_width - x - w, _max_y - y, w, idx);
    break;
  case 3:
    writeFastVLineCore(y, _width - x - w, w, idx);
    break;
  default: // case 0:
    writeFastHLineCore(x, y, w, idx);
  }
}

void Arduino_Canvas_Indexed::writeFastHLineCore(int16_t x, int16_t y,
                                                int16_t w, uint8_t idx)
{
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

        uint8_t *fb = _framebuffer + ((int32_t)y * WIDTH) + x;
        while (w--)
        {
          *(fb++) = idx;
        }
      }
    }
  }
}

void Arduino_Canvas_Indexed::writeFillRectPreclipped(int16_t x, int16_t y,
                                                     int16_t w, int16_t h, uint16_t color)
{
  uint8_t idx;
  if (_isDirectUseColorIndex)
  {
    idx = (uint8_t)color;
  }
  else
  {
    idx = get_color_index(color);
  }

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
  uint8_t *row = _framebuffer;
  row += y * WIDTH;
  row += x;
  for (int j = 0; j < h; j++)
  {
    for (int i = 0; i < w; i++)
    {
      row[i] = idx;
    }
    row += WIDTH;
  }
}

void Arduino_Canvas_Indexed::drawIndexedBitmap(
    int16_t x, int16_t y,
    uint8_t *bitmap, uint16_t *color_index, int16_t w, int16_t h, int16_t x_skip)
{
  if (_rotation > 0)
  {
    if (!_isDirectUseColorIndex)
    {
      Arduino_GFX::drawIndexedBitmap(x, y, bitmap, color_index, w, h, x_skip);
    }
    else
    {
      int32_t offset = 0;
      for (int16_t j = 0; j < h; j++, y++)
      {
        for (int16_t i = 0; i < w; i++)
        {
          writePixel(x + i, y, bitmap[offset++]);
        }
        offset += x_skip;
      }
    }
  }
  else
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
      uint8_t *row = _framebuffer;
      row += y * _width;
      row += x;
      int16_t i;
      int16_t wi;
      if (_isDirectUseColorIndex)
      {
        while (h--)
        {
          i = 0;
          wi = w;
          while (wi >= 4)
          {
            *((uint32_t *)&row[i]) = *((uint32_t *)bitmap);
            i += 4;
            wi -= 4;
            bitmap += 4;
          }
          while (i < w)
          {
            row[i++] = *bitmap++;
          }
          bitmap += x_skip;
          row += _width;
        }
      }
      else
      {
        while (h--)
        {
          for (int i = 0; i < w; i++)
          {
            row[i] = get_color_index(color_index[*bitmap++]);
          }
          bitmap += x_skip;
          row += _width;
        }
      }
    }
  }
}

void Arduino_Canvas_Indexed::drawIndexedBitmap(
    int16_t x, int16_t y,
    uint8_t *bitmap, uint16_t *color_index, uint8_t chroma_key, int16_t w, int16_t h, int16_t x_skip)
{
  if (_rotation > 0)
  {
    if (!_isDirectUseColorIndex)
    {
      Arduino_GFX::drawIndexedBitmap(x, y, bitmap, color_index, chroma_key, w, h, x_skip);
    }
    else
    {
      int32_t offset = 0;
      uint8_t color_key;
      for (int16_t j = 0; j < h; j++, y++)
      {
        for (int16_t i = 0; i < w; i++)
        {
          color_key = bitmap[offset++];
          if (color_key != chroma_key)
          {
            writePixel(x + i, y, color_key);
          }
        }
        offset += x_skip;
      }
    }
  }
  else
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
      uint8_t *row = _framebuffer;
      row += y * _width;
      row += x;
      uint8_t color_key;
      if (_isDirectUseColorIndex)
      {
        while (h--)
        {
          for (int i = 0; i < w; i++)
          {
            color_key = *bitmap++;
            if (color_key != chroma_key)
            {
              row[i] = color_key;
            }
          }
          bitmap += x_skip;
          row += _width;
        }
      }
      else
      {
        while (h--)
        {
          for (int i = 0; i < w; i++)
          {
            color_key = *bitmap++;
            if (color_key != chroma_key)
            {
              row[i] = get_color_index(color_index[color_key]);
            }
          }
          bitmap += x_skip;
          row += _width;
        }
      }
    }
  }
}

void Arduino_Canvas_Indexed::flush()
{
  _output->drawIndexedBitmap(_output_x, _output_y, _framebuffer, _color_index, WIDTH, HEIGHT);
}

uint8_t *Arduino_Canvas_Indexed::getFramebuffer()
{
  return _framebuffer;
}

uint16_t *Arduino_Canvas_Indexed::getColorIndex()
{
  return _color_index;
}

void Arduino_Canvas_Indexed::setDirectUseColorIndex(bool isEnable)
{
  _isDirectUseColorIndex = isEnable;
}

uint8_t Arduino_Canvas_Indexed::get_color_index(uint16_t color)
{
  color &= _color_mask;
  for (uint8_t i = 0; i < _indexed_size; i++)
  {
    if (_color_index[i] == color)
    {
      return i;
    }
  }
  if (_indexed_size == (COLOR_IDX_SIZE - 1)) // overflowed
  {
    raise_mask_level();
  }
  _color_index[_indexed_size] = color;
  // print("color_index[");
  // print(_indexed_size);
  // print("] = ");
  // println(color);
  return _indexed_size++;
}

INLINE uint16_t Arduino_Canvas_Indexed::get_index_color(uint8_t idx)
{
  return _color_index[idx];
}

void Arduino_Canvas_Indexed::raise_mask_level()
{
  if ((_current_mask_level + 1) < MAXMASKLEVEL)
  {
    int32_t buffer_size = _width * _height;
    uint8_t old_indexed_size = _indexed_size;
    uint8_t new_color;
    _indexed_size = 0;
    _color_mask = mask_level_list[++_current_mask_level];
    // print("Raised mask level: ");
    // println(_current_mask_level);

    // update _framebuffer color index, it is a time consuming job
    for (uint8_t old_color = 0; old_color < old_indexed_size; old_color++)
    {
      new_color = get_color_index(_color_index[old_color]);
      for (int32_t i = 0; i < buffer_size; i++)
      {
        if (_framebuffer[i] == old_color)
        {
          _framebuffer[i] = new_color;
        }
      }
    }
  }
}

#endif // !defined(LITTLE_FOOT_PRINT)
