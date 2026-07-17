<div align="center" markdown="1">
  <img src=".github/LilyGo_logo.png" alt="LilyGo logo" width="100"/>
</div>

<h1 align = "center">🌟T-Display-S3</h1>

<p align="center">
  <a href="./README.md">English</a> | <a href="./README_CN.md">中文</a>
</p>

[![PlatformIO CI](https://github.com/Xinyuan-LilyGO/T-Display-S3/actions/workflows/platformio.yml/badge.svg)](https://github.com/Xinyuan-LilyGO/T-Display-S3/actions/workflows/platformio.yml)
[![Arduino_CI](https://github.com/Xinyuan-LilyGO/T-Display-S3/actions/workflows/arduino_ci.yml/badge.svg)](https://github.com/Xinyuan-LilyGO/T-Display-S3/actions/workflows/arduino_ci.yml)

## 1. 支持的产品

| 产品 (PinMap)           | SOC        | Flash | PSRAM    | 分辨率   | 尺寸     |
| ----------------------- | ---------- | ----- | -------- | -------- | -------- |
| [T-Display-S3][1]       | ESP32-S3R8 | 16MB  | 8MB(OPI) | 170x320  | 1.9 英寸 |
| [T-Display-S3-Touch][2] | ESP32-S3R8 | 16MB  | 8MB(OPI) | 170x320  | 1.9 英寸 |
| [T-Display-S3-MIDI][3]  |            |       |          |          |          |

[1]: https://www.lilygo.cc/products/t-display-s3?variant=42589373268149
[2]: https://www.lilygo.cc/products/t-display-s3?variant=42351558590645
[3]: https://www.lilygo.cc/products/t-display-s3?variant=43164741632181

## 2. 示例程序

```bash
./examples/
├── Arduino_GFXDemo              # Arduino_GFX 示例
├── Arduino_GFX_PDQgraphicstest  # Arduino_GFX 示例
├── GetBatteryVoltage            # 获取电池电压示例
├── I2CScan                      # I2C 外部设备扫描
├── ImageScroll                  # 图片滚动示例 (by @Rudi Ackermann)
├── MPR121TouchSensor            # MPR121 电容触摸示例
├── PCBClock                     # TFT_eSPI PCBClock 示例 (by @VolosR)
├── PokerS3                      # TFT_eSPI PokerS3 示例 (by @VolosR)
├── SerialExample                # 串口通信示例
├── T-Display-S3-MIDI            # T-Display-S3-MIDI Shield 示例
├── T-Display-S3-BLE-Receiver    # BLE MIDI 无线接收器 + 钢琴显示 (ESP32_Host_MIDI)
├── T-Display-S3-BLE-Sender      # BLE MIDI 发送器 + 自动播放序列 (ESP32_Host_MIDI)
├── T-Display-S3-Piano           # USB MIDI 钢琴可视化 + 和弦检测 (ESP32_Host_MIDI + Gingoduino)
├── T-Display-S3-Piano-Debug     # MIDI 事件监控 + 原始数据包检测 (ESP32_Host_MIDI)
├── T-Display-S3-Gingoduino      # 实时和弦检测与和声分析 (ESP32_Host_MIDI + Gingoduino)
├── T-Display-S3-Queue           # MIDI 事件队列可视化调试 (ESP32_Host_MIDI)
├── T-Display-S3-USB-Device      # BLE 转 USB MIDI 桥接 (ESP32_Host_MIDI)
├── T-Display-S3-Music-Explorer  # 交互式音乐理论探索 + 音频合成 (Gingoduino)
├── TFT_Rainbow                  # TFT_eSPI 示例
├── factory                      # 出厂示例
├── lv_demos                     # LVGL 演示
├── nes                          # NES 游戏模拟器
├── ota                          # OTA 无线升级示例
├── sd                           # T-Display-TF Shield 示例
├── tft                          # TFT_eSPI 示例
├── touch_test                   # 电容触摸测试示例
├── usb_hid_pad                  # 电容触摸屏模拟 USB HID 示例
├── ULP_ADC                      # ULP-FSM ADC 检测 (arduino_esp32 版本: 3.0.0-rc3)
└── ULP_Count                    # ULP-FSM 寄存器计数 (arduino_esp32 版本: 3.0.0-rc3)
```

## 3. PlatformIO 快速开始（推荐）

1. 安装 [Visual Studio Code](https://code.visualstudio.com/) 和 [Python](https://www.python.org/)。
2. 在 VS Code 扩展市场中搜索 `PlatformIO` 插件并安装。
3. 安装完成后，重启 VS Code。
4. 在 VS Code 中，选择 `File` > `Open Folder` > 选择 `T-Display-S3` 目录。
5. 等待第三方依赖库安装完成。
6. 打开 `platformio.ini` 文件。
7. 取消注释其中一行 `default_envs = xxxx`，确保只有一行生效。
8. 点击左下角的（对勾）图标进行编译。
9. 通过 USB 将开发板连接到电脑。
10. 点击（箭头）上传固件。
11. 点击（插头图标）监控串口输出。
12. 如果无法上传或 USB 设备持续闪烁，请查看下方 **FAQ**。

> [!TIP]
> 如果遇到异常的编译错误，建议先重新克隆（或重新下载）本仓库。编译前请先点击 PlatformIO 工具栏中的 **Clean** 按钮清除旧的构建产物，然后再尝试编译。

## 4. Arduino IDE 手动安装

1. 安装 [Arduino IDE](https://www.arduino.cc/en/software)。
2. 在 Arduino 偏好设置中，打开设置选项卡，在 `Additional boards manager URLs` 输入框中输入以下 URL：
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
   > **注意：** 测试阶段使用的是 2.0.14 版本。高于 2.0.14 的版本可能无法正常运行，如遇到问题请降级。截至 2024/08/02，TFT_eSPI 在高于 2.0.14 的版本上无法工作（参见 [TFT_eSPI#3329](https://github.com/Bodmer/TFT_eSPI/issues/3329)）。

3. 下载 `T-Display-S3`，将其移动到 Arduino 库文件夹（例如 `C:\Users\YourName\Documents\Arduino\libraries`）。
4. 将 [lib 文件夹](./lib/) 中的所有文件夹复制到 Arduino 库文件夹。
5. 进入下载的 `T-Display-S3/examples` 目录。
6. 选择任意示例，双击 `.ino` 文件打开。
7. 在 Arduino IDE 中，打开 `Tools` 菜单，按以下表格配置：

    | Arduino IDE 设置项                    | 值                                |
    | ------------------------------------ | --------------------------------- |
    | Board                                | **ESP32S3 Dev Module**            |
    | Port                                 | 选择你的串口                       |
    | USB CDC On Boot                      | Enable                            |
    | CPU Frequency                        | 240MHz (WiFi)                     |
    | Core Debug Level                     | None                              |
    | USB DFU On Boot                      | Disable                           |
    | Erase All Flash Before Sketch Upload | Disable                           |
    | Events Run On                        | Core1                             |
    | Flash Mode                           | QIO 80MHz                         |
    | Flash Size                           | **16MB (128Mb)**                  |
    | Arduino Runs On                      | Core1                             |
    | USB Firmware MSC On Boot             | Disable                           |
    | Partition Scheme                     | **16M Flash (3M APP/9.9MB FATFS)**|
    | PSRAM                                | **OPI PSRAM**                     |
    | Upload Mode                          | **UART0/Hardware CDC**            |
    | Upload Speed                         | 921600                            |
    | USB Mode                             | **CDC and JTAG**                  |

    > **加粗** 的选项为必选项，其余根据实际情况选择。

8. 点击 `Upload`，等待编译和写入完成。
9. 如果无法上传或 USB 设备持续闪烁，请查看下方 **FAQ**。

- 也可以选择 `LilyGo T-Display-S3` 作为开发板，但分区表固定为 **16M Flash (3M APP/9.9MB FATFS)**。
- [T-Display-S3 Arduino IDE 视频教程](https://www.youtube.com/watch?v=PgtxisFvMcc)

## 5. ESP-IDF

T-Display-S3 的 ESP-IDF 版本示例，请前往 [LilyGo-Display-IDF](https://github.com/Xinyuan-LilyGO/LilyGo-Display-IDF)。

## 6. MicroPython

- [russhughes/st7789s3_mpy](https://github.com/russhughes/st7789s3_mpy)
- [lilygo-micropython](https://github.com/Xinyuan-LilyGO/lilygo-micropython)

## 引脚定义

| 名称          | GPIO | 说明              |
| ------------- | ---- | ----------------- |
| LCD BL        | 38   | 背光控制          |
| LCD D0        | 39   |                   |
| LCD D1        | 40   |                   |
| LCD D2        | 41   |                   |
| LCD D3        | 42   |                   |
| LCD D4        | 45   |                   |
| LCD D5        | 46   |                   |
| LCD D6        | 47   |                   |
| LCD D7        | 48   |                   |
| LCD RST       | 5    | 复位              |
| LCD CS        | 6    | 片选              |
| LCD DC        | 7    | 数据/命令         |
| LCD WR        | 8    | 写使能            |
| LCD RD        | 9    | 读使能            |
| Power On      | 15   | 外设电源控制      |
| Button 1      | 0    | BOOT 按钮         |
| Button 2      | 14   |                   |
| Battery Volt  | 4    | 电池电压 ADC      |
| IIC SCL       | 17   |                   |
| IIC SDA       | 18   |                   |
| Touch INT     | 16   | 触摸中断          |
| Touch RES     | 21   | 触摸复位          |
| SD CMD        | 13   | SD 卡             |
| SD CLK        | 11   | SD 卡             |
| SD D0         | 12   | SD 卡             |

> [!IMPORTANT]
>
> **注意：** GPIO15 是外设电源控制引脚，使用前必须将其设置为高电平，否则 LCD 和其他外设将无法工作。电池供电时请在 `setup()` 中添加：
> ```c
> pinMode(15, OUTPUT);
> digitalWrite(15, HIGH);
> ```
> SD 卡引脚（SD CMD/CLK/D0）仅适用于带 SD Shield 扩展板的型号，标准版 T-Display-S3 板载没有 SD 卡槽。

## 7. ESP32 基础示例

- [BLE 示例](https://github.com/espressif/arduino-esp32/tree/master/libraries/BLE)
- [WiFi 示例](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi)
- [SPIFFS 示例](https://github.com/espressif/arduino-esp32/tree/master/libraries/SPIFFS)
- [FFat 示例](https://github.com/espressif/arduino-esp32/tree/master/libraries/FFat)
- 更多 ESP32 芯片功能示例，请参阅 [arduino-esp32-libraries](https://github.com/espressif/arduino-esp32/tree/master/libraries)

## 8. 资料

| 产品 (PinMap)           | 原理图                                                 | 尺寸图                     | PCB 3D                                   | 引脚图                                  |
| ----------------------- | ------------------------------------------------------ | --------------------------- | ----------------------------------------- | ---------------------------------------- |
| [T-Display-S3][1]       | [原理图](./schematic/T_Display_S3.pdf)                | [DWG](./dimensions/PCB.dwg) | [STP](./dimensions/t-display-s3-full.stp) | [引脚图](./image/T-DISPLAY-S3.jpg)      |
| [T-Display-S3-Touch][2] | [原理图](./schematic/T_Display_S3.pdf)                | [DWG](./dimensions/PCB.dwg) | [STP](./dimensions/t-display-s3-full.stp) | [引脚图](./image/T-DISPLAY-S3-TOUCH.png)|
| [T-Display-S3-MIDI][3]  | [原理图](./schematic/SCH_T-Display-S3-MIDI_V1.1.pdf) | N.A                         | N.A                                       | N.A                                      |

## 9. 常见问题

1. **使用电池供电时屏幕不亮。**
   - 电池供电时，GPIO15 必须设置为高电平才能打开背光。
   - 请在 `setup()` 开头添加以下代码：
     ```cpp
     void setup() {
         // 打开显示屏电源
         pinMode(15, OUTPUT);
         digitalWrite(15, HIGH);
     }
     ```

2. **程序可以正常烧录，但屏幕没有显示。**
   - 如果使用的是 **TFT_eSPI**，可以先尝试运行 `Arduino_GFXDemo`。如果烧录后仍然没有显示，说明可能是硬件问题。
   - 如果 `Arduino_GFXDemo` 正常显示但 **TFT_eSPI** 不显示，说明 `User_Setup_Select` 配置可能被覆盖了。请参阅 FAQ 第 3 条重新配置 **TFT_eSPI**。

3. **如何更新 TFT_eSPI 或确认引脚配置是否正确？**
   - 在 Arduino IDE 库管理器中搜索 **TFT_eSPI** 并点击更新。
   - 进入 TFT_eSPI 安装目录（例如 `C:\Users\YourName\Documents\Arduino\libraries\TFT_eSPI`）。
   - 打开 `User_Setup_Select.h`，注释掉（或删除）默认的 `#include <User_Setup.h>` 行。
   - 找到 `Setup206_LilyGo_T_Display_S3.h`，取消注释，保存并关闭：
     ```cpp
     #include <User_Setups/Setup206_LilyGo_T_Display_S3.h>  // LilyGo T-Display S3 (ESP32S3 + ST7789 170x320 TFT)
     ```

4. **无法上传程序 — 请手动进入上传模式。**
   - 通过 USB 线连接开发板
   - 按住 **BOOT** 按钮
   - 在按住 **BOOT** 的同时，按下 **RST** 按钮
   - 松开 **RST**。
   - 松开 **BOOT**
   - 上传程序。
   - 按下 **RST** 退出下载模式

5. **如果使用外部电源而非 USB-C，请关闭 CDC 选项。** 因为开启 CDC 时，开发板在启动时会等待 USB 连接。
   - Arduino IDE：`Tools` > `USB CDC On Boot` > `Disable`
     > 关闭 USB CDC 后，串口将不再通过 USB-C 输出，而是通过 GPIO43 和 GPIO44 输出。
   - PlatformIO：在 `platformio.ini` 中添加以下配置：
     ```ini
     build_flags =
         ; -DARDUINO_USB_CDC_ON_BOOT=1   ; 开启：启动时打印并等待终端
         -UARDUINO_USB_CDC_ON_BOOT        ; 关闭：启动时不阻塞
     ```

6. **如果以上方法都无效，请刷写出厂固件快速验证。** 参见 [这里](https://github.com/Xinyuan-LilyGO/T-Display-S3/tree/main/firmware)。

7. **能否使用外部 5V 引脚供电？** 参见 [issues/205](https://github.com/Xinyuan-LilyGO/T-Display-S3/issues/205)。

8. **如何调整充电电流？** 默认充电电流为 500mA。参见 [issues/230](https://github.com/Xinyuan-LilyGO/T-Display-S3/issues/230)。

## LED 说明

| 功能         | 颜色 |
| ------------ | ---- |
| 充电指示灯   | 红色 |
| V3V 指示灯   | 绿色 |

### 充电指示灯状态

1. 未连接电池时闪烁或微亮。
2. 充电时常亮。
3. 充满电后熄灭。

### V3V 指示灯

1. USB 供电模式下，绿色指示灯常亮。
2. 电池供电模式下，GPIO15 为高时点亮，为低时熄灭。
3. V3V 控制排针的 V3V 输出，为屏幕供电。
