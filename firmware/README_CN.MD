# T-Display-S3 固件

[English Version](./README.MD)

本目录包含 T-Display-S3 系列开发板的出厂固件和测试工具，用于快速验证硬件是否正常工作。

## 固件列表

### T-Display-S3

| 固件 | 说明 | 下载 |
| --- | --- | --- |
| ScreenDetection | 屏幕检测工具 | [t-display-s3-screen-detect-20230315_0x0.bin](./t-display-s3-screen-detect-20230315_0x0.bin) |
| 出厂固件（无触摸） | 无触摸版本出厂固件 | [t-display-s3-no-touch-20231013_0x0.bin](./t-display-s3-no-touch-20231013_0x0.bin) |
| 出厂固件（触摸） | 触摸版本出厂固件 | [t-display-s3-touch-20230417_0x0.bin](./t-display-s3-touch-20230417_0x0.bin) |
| LVGL Demo | LVGL 图形界面演示 | [t-display-s3-lvgl-demo-20250109_0x0.bin](./t-display-s3-lvgl-demo-20250109_0x0.bin) |
| TFT_eSPI Demo | TFT_eSPI 库演示 | [t-display-s3-tft-espi-20231113_0x0.bin](./t-display-s3-tft-espi-20231113_0x0.bin) |

### T-Display-S3-MIDI

| 固件 | 说明 | 下载 |
| --- | --- | --- |
| MIDI 固件 v1.0.1 | 修复左右声道错误问题 | [t-display-s3-midi-v1.0.1-20231013_0x0.bin](./t-display-s3-midi-v1.0.1-20231013_0x0.bin) |
| MIDI 固件 v1.0.0 | 原始版本 | [t-display-s3-midi-v1.0.0-20231013_0x0.bin](./t-display-s3-midi-v1.0.0-20231013_0x0.bin) |

## 快速诊断硬件

**ScreenDetection** 是一个屏幕检测工具。刷入后通过串口监视器查看屏幕状态：

1. 刷入 [t-display-s3-screen-detect-20230315_0x0.bin](./t-display-s3-screen-detect-20230315_0x0.bin)
2. 打开 Arduino IDE 或 PlatformIO 的串口监视器
3. 按下 **RST** 按钮（开发板侧面）
4. 串口将自动重连并输出屏幕信息

![串口输出示例](./images/log.png)

> 如果串口输出信息正常但屏幕仍无显示，则屏幕可能已损坏。

## 刷写固件

### 进入下载模式

1. USB 连接开发板
2. 按住 **BOOT** 按钮
3. 按一下 **RST** 按钮
4. 松开 **RST** 按钮
5. 松开 **BOOT** 按钮
6. 刷写固件
7. 按 **RST** 按钮退出下载模式

### 方法 1: LILYGO Spark（推荐）

最简单的方式，下载 [LILYGO Spark](https://lilygo.cc/pages/lilygo-spark) 即可一键刷写。

![LILYGO Spark](./images/LILYGO-Spark.png)

### 方法 2: ESP Flash Download Tool

下载 [Flash Download Tool](https://docs.espressif.com/projects/esp-test-tools/en/latest/esp32/production_stage/tools/flash_download_tool.html)

![ESP Download Tool](./images/esp_downloader.gif)

刷写完成后按 **RST** 按钮重置。

### 方法 3: Web Flasher

在线刷写工具：[ESP Web Flasher](https://espressif.github.io/esptool-js/)

![Web Flasher](./images/web_flasher.gif)

刷写完成后按 **RST** 按钮重置。

### 方法 4: 命令行

安装 esptool：

```bash
python3 -m pip install --upgrade pip
python3 -m pip install esptool
```

**ESP32 刷写命令：**

```bash
esptool --chip esp32 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m 0x0 firmware.bin
```

**ESP32-S3 刷写命令：**

```bash
esptool --chip esp32s3 --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m 0x0 firmware.bin
```

## 相关资源

- [Espressif 官方文档](https://docs.espressif.com/projects/esptool/)
- [LILYGO 官网](https://www.lilygo.cc/)
