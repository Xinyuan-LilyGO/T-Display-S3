<h1 align = "center"> 🌟T-Display-S3🌟</h1>

[![PlatformIO CI](https://github.com/Xinyuan-LilyGO/T-Display-S3/actions/workflows/platformio.yml/badge.svg)](https://github.com/Xinyuan-LilyGO/T-Display-S3/actions/workflows/platformio.yml)
[![Arduino_CI](https://github.com/Xinyuan-LilyGO/T-Display-S3/actions/workflows/arduino_ci.yml/badge.svg)](https://github.com/Xinyuan-LilyGO/T-Display-S3/actions/workflows/arduino_ci.yml)

## 1️⃣Support Product

| Product (PinMap)        | SOC        | Flash | PSRAM    | Resolution | Size     |
| ----------------------- | ---------- | ----- | -------- | ---------- | -------- |
| [T-Display-S3][1]       | ESP32-S3R8 | 16MB  | 8MB(OPI) | 170x320    | 1.9 Inch |
| [T-Display-S3-Touch][2] | ESP32-S3R8 | 16MB  | 8MB(OPI) | 170x320    | 1.9 Inch |
| [T-Display-S3-MIDI][3]  |            |       |          |            |          |

[1]: https://www.lilygo.cc/products/t-display-s3?variant=42589373268149
[2]: https://www.lilygo.cc/products/t-display-s3?variant=42351558590645
[3]: https://www.lilygo.cc/products/t-display-s3?variant=43164741632181

## 2️⃣Quick Start

* **Arduino IDE**: 
  1. Install [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32).
  2. Copy all libraries from the `lib` folder to your Arduino libraries directory.
  3. Select Board: **ESP32S3 Dev Module**.
  4. Enable **USB CDC On Boot** (to see Serial output) and set **Flash Mode** to OPI.
* **PlatformIO**: Install the PlatformIO extension in VS Code, open this project folder, and click "Upload".

## 3️⃣Examples

```
./examples/
├── Arduino_GFXDemo              #  Arduino_GFX example
├── Arduino_GFX_PDQgraphicstest  #  Arduino_GFX example
├── GetBatteryVoltage            #  Get battery voltage example
├── I2CScan                      #  Scan for external devices using I2C
├── ImageScroll                  #  Image scrolling example by @Rudi Ackermann
├── MPR121TouchSensor            #  Example of using MPR121 capacitive touch
├── PCBClock                     #  TFT_eSPI PCBClock example by @VolosR
├── PokerS3                      #  TFT_eSPI PokerS3  example by @VolosR
├── SerialExample                #  Example of using serial communication
├── T-Display-S3-MIDI            #  T-Display-S3-MIDI Shield example
├── T-Display-S3-BLE-Receiver     #  Wireless BLE MIDI receiver with piano display (ESP32_Host_MIDI)
├── T-Display-S3-BLE-Sender       #  BLE MIDI sender with auto-play sequences (ESP32_Host_MIDI)
├── T-Display-S3-Piano           #  USB MIDI Piano visualizer with chord detection (ESP32_Host_MIDI + Gingoduino)
├── T-Display-S3-BLE-Receiver     #  Wireless BLE MIDI receiver with piano display (ESP32_Host_MIDI)
├── T-Display-S3-BLE-Sender       #  BLE MIDI sender with auto-play sequences (ESP32_Host_MIDI)
├── T-Display-S3-Piano-Debug      #  MIDI event monitor with raw packet inspector (ESP32_Host_MIDI)
├── T-Display-S3-Gingoduino       #  Real-time chord detection and harmonic analysis (ESP32_Host_MIDI + Gingoduino)
├── T-Display-S3-Queue            #  MIDI event queue visualizer for debugging (ESP32_Host_MIDI)
├── T-Display-S3-USB-Device       #  BLE-to-USB MIDI bridge — T-Display-S3 as USB MIDI interface (ESP32_Host_MIDI)
├── T-Display-S3-Music-Explorer   #  Interactive music theory explorer with audio synthesis (Gingoduino)
├── TFT_Rainbow                  #  TFT_eSPI example
├── factory                      #  factory example
├── lv_demos                     #  lvgl demo
```