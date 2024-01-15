# About the canvas mono class of the GFX library

There are some Display Chips on the market that can display a small amount of graphic data but
don't offer much functionality on it's own on other than data transfer. Examples are the OLED
Displays Chips SSD1306 or SH1106.

These displays need an in memory copy of the graphics and updating the display is implemented by
a bulk data transfer of the internal memory to the display.

Therefore the Canvas Mono class must be initialized in a way the internal memory fits to the
memory inside the graphic driver chip including the width and height of the display memory and
the orientation of a byte to either horizontal or vertical.

Bei changing the orientation of the canvas the graphics elements like lines or characters are positioned on the display memory accordingly. When the rotating in 90 or 270 degrees der width and height of the logical display will be swapped but not the memory organization.

