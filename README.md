<h1 align = "center"> 🌟T-Display-S3🌟</h1>

<img  width="480" src=image/logo.png>

## Description

T-Display-S3 is a development board whose main control chip is ESP32-S3. It is equipped with a 1.9-inch LCD color screen and two programmable buttons.Communication using I8080 interface Retains the same layout design as T-Display. You can directly use ESP32S3 for USB communication or programming.

***
## Pinmap

<img  width="480" src=image/T-DISPLAY-S3.jpg>

***
<h3 align = "left">Product 📷:</h3>

|  Product |  Product Link |
| :--------: | :---------: |
| T-Display-S3 |  [AliExpress]()   |


***
## Quick Start

> Arduino:
>- Click "File" in the upper left corner -> Preferences -> Additional Development >Board Manager URL -> Enter the URL in the input box.
(ESP32S3 is a new chip, and the SDK version needs to be version 2.0.3 or above)
> `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pagespackage_esp32_index.json`
>-  Click OK and the software will be installed by itself. After installation, restart the Arduino IDE software.
>- Copy all files in the lib folder to `\Arduino\libraries`

> PlatfromIO:
> - PlatformIO plug-in installation: Click on the extension on the left column -> search platformIO -> install the first plug-in
> - Click Platforms -> Embedded -> search Espressif 32 in the input box -> select the corresponding firmware installation

> ESP-IDF:
> - The installation method is also inconsistent depending on the system, it is recommended to refer to the [official manual](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html) for installation


***
## Quick question and answer
1. The TFT_eSPI library used does not display properly?
> The TFT_eSPI library needs to modify some settings. It has been modified in the lib. Please use this library or add Micky_commit.patch to the local library.

