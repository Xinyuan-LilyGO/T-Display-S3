#include "ST7789_Handler.h"
#include "mapping.h"
#include <vector>

ST7789_Handler display;

#ifndef TFT_GRAY
#define TFT_GRAY 0x8410
#endif

ST7789_Handler::ST7789_Handler() {
  // Simple constructor (can be expanded if needed)
}

void ST7789_Handler::init() {
  tft.init();
  tft.setRotation(2);   // Rotate 180 degrees for orientation correction
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE);
  tft.fillScreen(TFT_BLACK);

  // Extra backlight guarantee (pin 38 on T-Display S3).
  // LovyanGFX configures via PWM, but when running on battery
  // the pin may not initialize correctly.
  // Ref: Discussion #8 - AtmosphEng (GPIO 15 HIGH for battery backlight)
  pinMode(TFT_BL_PIN, OUTPUT);
  digitalWrite(TFT_BL_PIN, HIGH);
}

// Overload to display a single string
void ST7789_Handler::print(const std::string& message) {
    static std::string lastMessage = "";

    // Skip redraw if message hasn't changed (avoids flickering)
    if (lastMessage == message) {
        return;
    }
    lastMessage = message;

    tft.fillScreen(TFT_BLACK);

    const int margin = 2;
    const int availableHeight = 240 - margin;
    const int totalLines = 12;
    const int lineHeight = availableHeight / totalLines;

    int x = 2;
    int y = margin / 2;

    // Split string into multiple lines
    String text(message.c_str());
    String lines[totalLines];
    int lineCount = 0;
    int start = 0;
    int idx = text.indexOf('\n');
    while (idx != -1 && lineCount < totalLines) {
        lines[lineCount++] = text.substring(start, idx);
        start = idx + 1;
        idx = text.indexOf('\n', start);
    }
    if (lineCount < totalLines && start < text.length()) {
        lines[lineCount++] = text.substring(start);
    }

    // Display each line with proper spacing
    for (int i = 0; i < lineCount; i++) {
        tft.setTextSize(1);
        tft.setTextColor(TFT_WHITE);
        tft.drawString(lines[i], x, y);
        y += lineHeight - 2;
    }
}

// Overload to display a vector of strings
void ST7789_Handler::print(const std::vector<std::string>& messages) {
    static std::string lastMessage = "";

    // Concatenate vector elements into a single string separated by ", "
    std::string concatenatedMessage;
    for (size_t i = 0; i < messages.size(); ++i) {
        concatenatedMessage += messages[i];
        if (i < messages.size() - 1) {
            concatenatedMessage += ", ";
        }
    }

    // Skip redraw if message hasn't changed (avoids flickering)
    if (lastMessage == concatenatedMessage) {
        return;
    }
    lastMessage = concatenatedMessage;

    print(concatenatedMessage);
}

// Overload to display a string splitting by comma
void ST7789_Handler::println(const std::string& message) {
    static std::string lastMessage = "";

    // Skip redraw if message hasn't changed (avoids flickering)
    if (lastMessage == message) {
        return;
    }
    lastMessage = message;

    tft.fillScreen(TFT_BLACK);

    const int margin = 2;
    const int availableHeight = 240 - margin;
    const int totalLines = 12;
    const int lineHeight = availableHeight / totalLines;

    int x = 2;
    int y = margin / 2;

    // Split string into lines by ","
    String text(message.c_str());
    String lines[totalLines];
    int lineCount = 0;
    int start = 0;
    int idx = text.indexOf(',');
    while (idx != -1 && lineCount < totalLines) {
        lines[lineCount++] = text.substring(start, idx);
        start = idx + 1;
        idx = text.indexOf(',', start);
    }
    if (lineCount < totalLines && start < text.length()) {
        lines[lineCount++] = text.substring(start);
    }

    // Display each line with proper spacing
    for (int i = 0; i < lineCount; i++) {
        tft.setTextSize(1);
        tft.setTextColor(TFT_WHITE);
        tft.drawString(lines[i], x, y);
        y += lineHeight - 2;
    }
}

// Overload to display a vector of strings, each element on a new line
void ST7789_Handler::println(const std::vector<std::string>& messages) {
    static std::string lastMessage = "";

    // Concatenate vector elements to check for changes
    std::string concatenatedMessage;
    for (const auto& msg : messages) {
        concatenatedMessage += msg + ",";
    }

    // Skip redraw if message hasn't changed (avoids flickering)
    if (lastMessage == concatenatedMessage) {
        return;
    }
    lastMessage = concatenatedMessage;

    println(concatenatedMessage);
}

void ST7789_Handler::clear() {
  tft.fillScreen(TFT_BLACK);
}
