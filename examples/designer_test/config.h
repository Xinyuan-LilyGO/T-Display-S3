#pragma once

// WiFi credentials — edit before flashing.
// If WiFi connection fails the device will fall back to AP mode:
//   SSID: "designer-test"
//   IP:   192.168.4.1
#define WIFI_SSID     ""
#define WIFI_PASSWORD ""

// AP fallback settings
#define AP_SSID       "designer-test"
#define AP_PASSWORD   ""          // open network

// Directory on LittleFS where images are stored
#define IMAGES_DIR    "/images"
