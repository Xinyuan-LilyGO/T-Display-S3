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

## 2️⃣Examples

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
├── TFT_Rainbow                  #  TFT_eSPI example
├── factory                      #  factory example
├── lv_demos                     #  lvgl demo                        
├── nes                          #  NES game emulator
├── ota                          #  Over-the-air upgrade example
├── sd                           #  T-Display-TF Shield example
├── tft                          #  TFT_eSPI example
├── touch_test                   #  Capacitive touch test example
└── usb_hid_pad                  #  Capacitive Touch Screen Simulation USB HID Example
```

## 3️⃣ PlatformIO Quick Start (Recommended)

1. Install [Visual Studio Code](https://code.visualstudio.com/) and [Python](https://www.python.org/)
2. Search for the `PlatformIO` plugin in the `VisualStudioCode` extension and install it.
3. After the installation is complete, you need to restart `VisualStudioCode`
4. After restarting `VisualStudioCode`, select `File` in the upper left corner of `VisualStudioCode` -> `Open Folder` -> select the `LilyGO AMOLED Series` directory
5. Wait for the installation of third-party dependent libraries to complete
6. Click on the `platformio.ini` file, and in the `platformio` column
7. Uncomment one of the lines `default_envs = xxxx` to make sure only one line works
8. Click the (✔) symbol in the lower left corner to compile
9. Connect the board to the computer USB
10. Click (→) to upload firmware
11. Click (plug symbol) to monitor serial output
12. If it cannot be written, or the USB device keeps flashing, please check the **FAQ** below


## 4️⃣  Arduino IDE Manual installation

1. Install [Arduino IDE](https://www.arduino.cc/en/software)
2. Install [Arduino ESP32 V 2.0.5 or above and below V3.0](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
3. Download `T-Display-S3` , move to Arduino library folder (e.g. C:\Users\YourName\Documents\Arduino\libraries)
4. Copy all folders in [lib folder](./lib/) to Arduino library folder (e.g. C:\Users\YourName\Documents\Arduino\libraries)
5. Enter the downloaded `T-Display-S3/examples` directory
6. Select any example and double-click the `any_example.ino` to open it
7. Open ArduinoIDE ,`Tools` ，Make your selection according to the table below
    | Arduino IDE Setting                  | Value                             |
    | ------------------------------------ | --------------------------------- |
    | Board                                | **ESP32S3 Dev Module**            |
    | Port                                 | Your port                         |
    | USB CDC On Boot                      | Enable                            |
    | CPU Frequency                        | 240MHZ(WiFi)                      |
    | Core Debug Level                     | None                              |
    | USB DFU On Boot                      | Disable                           |
    | Erase All Flash Before Sketch Upload | Disable                           |
    | Events Run On                        | Core1                             |
    | Flash Mode                           | QIO 80MHZ                         |
    | Flash Size                           | **16MB(128Mb)**                   |
    | Arduino Runs On                      | Core1                             |
    | USB Firmware MSC On Boot             | Disable                           |
    | Partition Scheme                     | **16M Flash(3M APP/9.9MB FATFS)** |
    | PSRAM                                | **OPI PSRAM**                     |
    | Upload Mode                          | **UART0/Hardware CDC**            |
    | Upload Speed                         | 921600                            |
    | USB Mode                             | **CDC and JTAG**                  |
    * The options in bold are required, others are selected according to actual conditions.

8. Click `upload` , Wait for compilation and writing to complete
9. If it cannot be written, or the USB device keeps flashing, please check the **FAQ** below

* You can also choose `LilyGo T-Display-S3` as the board, but the partition table is fixed to **16M Flash (3M APP/9.9MB FATFS)**
* [T-Display-S3 Arduino IDE Record](https://www.youtube.com/watch?v=PgtxisFvMcc)

## 5️⃣ ESP-IDF

* `T-Display-S3` esp-idf version example, please jump to this [LilyGo-Display-IDF](https://github.com/Xinyuan-LilyGO/LilyGo-Display-IDF)

## 6️⃣ Micropython

* [russhughes/st7789s3_mpy](https://github.com/russhughes/st7789s3_mpy)
* [Micropython](https://github.com/Xinyuan-LilyGO/lilygo-micropython)


# 7️⃣ ESP32 basic examples

* [BLE Examples](https://github.com/espressif/arduino-esp32/tree/master/libraries/BLE)
* [WiFi Examples](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi)
* [SPIFFS Examples](https://github.com/espressif/arduino-esp32/tree/master/libraries/SPIFFS)
* [FFat Examples](https://github.com/espressif/arduino-esp32/tree/master/libraries/FFat)
* For more examples of esp32 chip functions, please refer to [arduino-esp32-libraries](https://github.com/espressif/arduino-esp32/tree/master/libraries)


# 8️⃣ Resource

| Product(PinMap)         | schematic                                               | Dimensions                  | PCB 3D                                    | PinMap                                   |
| ----------------------- | ------------------------------------------------------- | --------------------------- | ----------------------------------------- | ---------------------------------------- |
| [T-Display-S3][1]       | [schematic](./schematic/T_Display_S3.pdf)               | [DWG](./dimensions/PCB.dwg) | [STP](./dimensions/t-display-s3-full.stp) | [PinMap](./image/T-DISPLAY-S3.png)       |
| [T-Display-S3 Touch][1] | [schematic](./schematic/T_Display_S3.pdf)               | [DWG](./dimensions/PCB.dwg) | [STP](./dimensions/t-display-s3-full.stp) | [PinMap](./image/T-DISPLAY-S3-TOUCH.jpg) |
| [T-Display-S3-MIDI][1]  | [schematic](./schematic/SCH_T-Display-S3-MIDI_V1.1.pdf) | DWG                         | STP                                       |                                          |


## 9️⃣ FAQ

1. **The screen does not light up when using battery?**
   * When T-Display-S3 is powered by battery, GPIO15 must be set to HIGH to turn on the backlight.
   * Please add the following two lines at the beginning of the setup
   ```C++
   void setup(){
      //Turn on display power
      pinMode(15, OUTPUT);
      digitalWrite(15, HIGH);
   }
   
   ```
2.  **The program can be written normally, but there is still no display after writing**
   * If you are using **TFT_eSPI**, then you can try running `Arduino_GFXDemo` first. If nothing is displayed after writing, you can determine that there is a problem with the hardware.
   * If `Arduino_GFXDemo` is written normally, but **TFT_eSPI** is not displayed, then it can be judged that `User_Setup_Select` has been overwritten, then please read the third article of **FAQ** to reconfigure **TFT_eSPI**
3. **How to update **TFT_eSPI**, or confirm whether the **TFT_eSPI** pin configuration is correct?**
   * Search for **TFT_eSPI** in the ArduinoIDE library manager and click Update.
   * Enter the default library manager installation location and open the **TFT_eSPI** folder. The default installation location is:(e.g. C:\Users\YourName\Documents\Arduino\libraries)
   * Open User_Setup_Select.h, comment out #include <User_Setup.h> which is enabled by default, or delete it
   * Search **Setup206_LilyGo_T_Display_S3**, find it, cancel the previous comment, then save it, and finally close it, so that TFT_eSPI uses the pin definition of T-Display-S3 by default
   ```c++
   #include <User_Setups/Setup206_LilyGo_T_Display_S3.h>     // For the LilyGo T-Display S3 based ESP32S3 with ST7789 170 x 320 TFT
   ```
4. **Can't upload any sketch，Please enter the upload mode manually.**
   * Connect the board via the USB cable
   * Press and hold the **BOOT** button , While still pressing the **BOOT** button
   * Press **RST** button
   * Release the **RST** button
   * Release the **BOOT** button (If there is no **BOOT** button, disconnect IO0 from GND.)
   * Upload sketch
   * Press the **RST** button to exit download mode

5. **If you use external power supply instead of USB-C, please turn off the CDC option. This is because the board will wait for USB access when it starts.**

   * For Arduino IDE users, it can be turned off in the options , Please note that turning off USB CDC will turn off Serial redirection to USBC. At this time, you will not see any Serial message output when opening the port from USB-C, but output from GPIO43 and GPIO44.

   ```c
   Tools -> USB CDC On Boot -> Disable
   ```

   * For platformio users, you can add the following compilation flags in the ini file

   ```c
   build_flags =
       ; Enable -DARDUINO_USB_CDC_ON_BOOT will start printing and wait for terminal access during startup
       ; -DARDUINO_USB_CDC_ON_BOOT=1

       ; Enable -UARDUINO_USB_CDC_ON_BOOT will turn off printing and will not block when using the battery
       -UARDUINO_USB_CDC_ON_BOOT
   ```

6. **If all the above are invalid, please flash the factory firmware for quick verification, please check [here](./firmware/README.MD)**
7. **Can I use an external 5V pin for power? Please see here [issues/205](https://github.com/Xinyuan-LilyGO/T-Display-S3/issues/205)**
9. The default charging current is set at 500mA per hour. If you need to adjust the charging current, please see this [issue](https://github.com/Xinyuan-LilyGO/T-Display-S3/issues/230)

