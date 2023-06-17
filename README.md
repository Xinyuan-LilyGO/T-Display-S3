<h1 align = "center"> 🌟T-Display-S3🌟</h1>

## Description

T-Display-S3 is a ESP32-S3 development board. It is equipped with a color 1.9" LCD screen and two programmable buttons. Communication using I8080 interface. Retains the same layout design as T-Display. You can directly use ESP32S3 for USB communication or programming.

## T-Display-S3 Pinout

![](image/T-DISPLAY-S3.jpg)

## T-Display-S3 TOUCH Pinout

![](image/T-DISPLAY-S3-TOUCH.png)


## Where to buy

T-Display-S3 is available from: [LILYGO](https://www.lilygo.cc/products/t-display-s3), [Amazon](https://www.amazon.com/dp/B0B7X5RVTH?ref=myi_title_dp) and [Aliexpress](https://www.aliexpress.com/item/3256804310228562.html).

## Quick Start

The board uses USB as the JTAG upload port. When printing serial port information on USB, CDC_ON_BOOT configuration needs to be turned on. 
If the port cannot be found when uploading the program or the USB has been used for other functions, the port does not appear. 
Please enter the upload mode manually. 

1. Connect the board via the USB cable
2. Press and hold the BOOT button , While still pressing the BOOT button, press RST
3. Release the RST
4. Release the BOOT button
5. Upload sketch

### Arduino

1. In Arduino Preferences, on the Settings tab, enter the `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` URL in the `Additional boards manager URLs` input box. 
2. Click OK and the software will install. 
3. Search for ESP32 in Tools → Board Manager and install `esp32` (Arduino-ESP32)
4. Copy everything from this repository lib folder to [Arduino library folder](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries#manual-installation) (e.g. `C:\Users\YourName\Documents\Arduino\libraries`)
5. Select the correct settings in the Tools menu as shown below.

| Setting                  | Value                            |
| :----------------------- | :------------------------------- |
| USB CDC On Boot          | Enabled                          |
| CPU Frequency            | 240MHz (WiFi)                    |
| Core Debug Level         | None                             |
| USB DFU On Boot          | Enabled                          |
| Events Run On            | Core 1                           |
| Flash Mode               | QIO 80MHz                        |
| Flash Size               | 16MB (128Mb)                     |
| JTAG Adapter             | Integrated USB JTAG              |
| Arduino Runs On          | Core 1                           |
| USB Firmware MSC On Boot | Disabled                         |
| Partition Scheme         | Huge APP (3MB No OTA/1MB SPIFFS) |
| PSRAM                    | OPI PSRAM                        |
| USB Mode                 | Hardware CDC and JTAG            |

### PlatformIO

1. PlatformIO plug-in installation: click on the extension on the left column → search platformIO → install the first plug-in
2. Click Platforms → Embedded → search Espressif 32 in the input box → select the corresponding firmware installation

### ESP-IDF

The installation method varies depending on the system, refer to the [official manual](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) for installation.


### Micropython

See [Micropython](https://github.com/Xinyuan-LilyGO/lilygo-micropython)

## Libraries

|                                                     Library | Minimum version | Description                |
| ----------------------------------------------------------: | :-------------- | :------------------------- |
| [Arduino-ESP32](https://github.com/espressif/arduino-esp32) | 2.0.3           | ESP32 Arduino core         |
|              [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) | 2.5.0           | graphics and fonts library |

## Tips

1. Use board as delivered. Connect board to usb to computer without
touching anything else. Upload the code. The code will start right after
download and run. Unplug the board and connect to usb again. The board
will not do anything.

2. Use board as delivered. Push down boot button. While boot button is
pushed, connect usb to computer. Release boot button. Upload the code.
The code will not start right after download. Nothing will happen.
Unplug the board and connect to usb again.The board will now boot the
code. Unplug the board from USB to computer and connect to other power
source like LiPO battery. Again the board will start the code. The
computer is not needed again.

New code can be loaded to the board by going to step 1 or 2. Both will
still work.

3. The program can be written normally, but there is still no display after writing
   1. There are factory test files in the firmware folder, which can be flashed into the board to check whether the board is normal. If there is still no display, then it can be judged that there is a problem with the board or the screen
   2. Delete the <TFT_eSPI> in the libraries, and replace the <TFT_eSPI> in the <lib> folder of the main page to the libraries directory
   3. When opening the Arduino IDE, it prompts whether to upgrade the library, please choose not to upgrade, otherwise it will overwrite the configuration of the <TFT_eSPI> display