#include <esp_task_wdt.h>
#include <esp_wifi.h>

#include "Arduino.h"
#include "img_logo.h"
#include "pin_config.h"

#include "Arduino_GFX_Library.h" /* https://github.com/moononournation/Arduino_GFX */

#include <FS.h>
#include <SPIFFS.h>

extern "C" {
#include <nes/nes.h> /* https://github.com/moononournation/arduino-nofrendo */
#include <nofrendo.h>
}

#define FSROOT "/fs"

Arduino_DataBus *bus = new Arduino_ESP32LCD8(7 /* DC */, 6 /* CS */, 8 /* WR */, 9 /* RD */, 39 /* D0 */, 40 /* D1 */, 41 /* D2 */, 42 /* D3 */,
                                             45 /* D4 */, 46 /* D5 */, 47 /* D6 */, 48 /* D7 */);
Arduino_GFX *gfx = new Arduino_ST7789(bus, 5 /* RST */, 0 /* rotation */, true /* IPS */, 170 /* width */, 320 /* height */, 35 /* col offset 1 */,
                                      0 /* row offset 1 */, 35 /* col offset 2 */, 0 /* row offset 2 */);

static int16_t frame_x, frame_y;
extern uint16_t myPalette[];
int16_t bg_color;

void setup() {
  pinMode(PIN_POWER_ON, OUTPUT);
  digitalWrite(PIN_POWER_ON, HIGH);
  ledcSetup(0, 2000, 8);
  ledcAttachPin(PIN_LCD_BL, 0);
  ledcWrite(0, 255); /* Screen brightness can be modified by adjusting this parameter. (0-255) */

  Serial.begin(115200);
  Serial.println("Hello T-Display-S3 NES DEMO");

  // turn off WiFi
  esp_wifi_deinit();

  // disable Core 0 WDT
  TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
  esp_task_wdt_delete(idle_0);

  gfx->begin();
  gfx->setRotation(1);
  bg_color = gfx->color565(24, 28, 24); // DARK DARK GREY

  gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)img_logo, 320, 172);
  delay(2000);

  SPIFFS.begin(true, FSROOT);

  FS filesystem = SPIFFS;

  // find first rom file (*.nes)
  File root = filesystem.open("/");
  char *argv[1];
  if (!root) {
    Serial.println("Filesystem mount failed!");
  } else {
    bool foundRom = false;

    File file = root.openNextFile();
    while (file) {
      if (file.isDirectory()) {
        // skip
      } else {
        char *filename = (char *)file.name();
        int8_t len = strlen(filename);
        if (strstr(strlwr(filename + (len - 4)), ".nes")) {
          foundRom = true;
          char fullFilename[256];
          sprintf(fullFilename, "%s/%s", FSROOT, filename);
          Serial.println(fullFilename);
          argv[0] = fullFilename;
          break;
        }
      }

      file = root.openNextFile();
    }

    if (!foundRom) {
      Serial.println("Failed to find rom file, please copy rom file to data folder and upload with \"ESP32 Sketch Data Upload\"");
      argv[0] = "/";
    }

    Serial.println("NoFrendo start!\n");
    nofrendo_main(1, argv);
    Serial.println("NoFrendo end!\n");
  }
}
void loop(void) { delay(1); }

extern "C" void display_init() {
  frame_x = (gfx->width() - NES_SCREEN_WIDTH) / 2;
  frame_y = 35;
}

extern "C" void display_write_frame(const uint8_t *data[]) {
  gfx->startWrite();
  bus->writeC8D16D16(0x2A, frame_x, frame_x + NES_SCREEN_WIDTH - 1);
  bus->writeC8D16D16(0x2B, frame_y, frame_y + NES_SCREEN_HEIGHT - 1);
  bus->writeCommand(0x2c);
  for (int32_t i = 0; i < NES_SCREEN_HEIGHT; i++) {
    if ((i % 7) < 5)
      bus->writeIndexedPixels((uint8_t *)(data[i]), myPalette, NES_SCREEN_WIDTH);
  }
  gfx->endWrite();
}

extern "C" void display_clear() { gfx->fillScreen(bg_color); }