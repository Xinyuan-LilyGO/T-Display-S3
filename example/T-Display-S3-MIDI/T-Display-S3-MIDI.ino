/*
 * @Description: This program is used to test the T-Display-S3-MIDI program and consists
of 3 parts, 1.PCA9535_Test, 2.MPR121_Test, 3.PCM5102A_Test

 * @version: V1.0.1
 * @Author: LILYGO_L
 * @Date: 2023-06-12 16:16:37
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-09-05 13:44:30
 */

#include "Arduino.h"
#include "TFT_eSPI.h"
#include "pin_config.h"
#include "Adafruit_MPR121.h"
#include "FS.h"
#include "SPI.h"
#include "SD.h"
#include "Audio.h"
#include "PCA95x5.h"

#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)
#error  "The current version is not supported for the time being, please use a version below Arduino ESP32 3.0"
#endif

#define MPR121_PCA95x5_Count 1

TFT_eSPI tft;
Audio audio;
int32_t Windows = 0;

// Function interrupt
void PCA9535_MPR121_External_Interrupt();

Adafruit_MPR121 Touch_Sensor1 = Adafruit_MPR121();
Adafruit_MPR121 Touch_Sensor2 = Adafruit_MPR121();
volatile bool MPR121_External_Interrupt_Flag = false;
volatile bool PCA95x5_External_Interrupt_Flag = false;

uint16_t touched1 = 0;
uint16_t touched2 = 0;
uint16_t MPR121_IRQ_Number = 0;

File File_CurrentFile;
File File_Root;
bool File_Scan_Flag = true;

PCA9535 PCA9535_Class;

void PCA9535_MPR121_External_Interrupt()
{
    // Interrupt called (No Serial no read no wire in this function, and DEBUG disabled on PCF library)
    PCA95x5_External_Interrupt_Flag = true;
    MPR121_External_Interrupt_Flag = true;
}

void deviceScan(TwoWire *_port, Stream *stream)
{
    uint8_t err, addr;
    int nDevices = 0;
    for (addr = 1; addr < 127; addr++) {
        _port->beginTransmission(addr);
        err = _port->endTransmission();
        if (err == 0) {
            stream->print("I2C device found at address 0x");
            if (addr < 16)
                stream->print("0");
            stream->print(addr, HEX);
            stream->println(" !");
            nDevices++;
        } else if (err == 4) {
            stream->print("Unknow error at address 0x");
            if (addr < 16)
                stream->print("0");
            stream->println(addr, HEX);
        }
    }
    if (nDevices == 0)
        stream->println("No I2C devices found\n");
    else
        stream->println("Done\n");
}

const char *PCA95x5_TFT_Show(uint8_t Num)
{
    if (Num == 0) {
        return "LOW";
    } else {
        return "HIGH";
    }
}

/**
 * @brief PCA9535 test program, detect the mode of IO ports, and measure the voltage
level of the corresponding IO ports using a multimeter.

 * @return
 * @Date 2023-07-13 10:06:19
 */
void PCA95x5_Test(void)
{
    PCA95x5_External_Interrupt_Flag = false;

    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_VIOLET);
    tft.drawString("START", 0, 5, 4);
    tft.setTextColor(TFT_BROWN);
    // tft.drawString("|---PCA9535_MPR121---|", 100, 10, 2);
    tft.drawString("|---TDS3_MIDI_Driver---|", 90, 10, 2);
    tft.drawString("LILYGO", 260, 5, 1);
    tft.drawString("V1.0.0", 260, 17, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("READ VALUE FROM PCA95x5: ", 5, 30, 2);

    tft.setTextColor(TFT_PINK);
    tft.drawString("PORT 0", 50, 50, 1);
    tft.setTextColor(TFT_WHITE);

    tft.setTextColor(TFT_PINK);
    tft.drawString("PORT 1", 210, 50, 1);
    tft.setTextColor(TFT_WHITE);

    tft.drawString("GPIO_00", 20, 60, 1);
    tft.drawString("->", 75, 60, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100, 60, 1);
    tft.drawString("HIGH", 100, 60, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P00)), 100, 60, 1);

    tft.drawString("GPIO_01", 20, 74, 1);
    tft.drawString("->", 75, 74, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100, 74, 1);
    tft.drawString("HIGH", 100, 74, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P01)), 100, 74, 1);

    tft.drawString("GPIO_02", 20, 88, 1);
    tft.drawString("->", 75, 88, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100, 88, 1);
    tft.drawString("HIGH", 100, 88, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P02)), 100, 88, 1);

    tft.drawString("GPIO_03", 20, 102, 1);
    tft.drawString("->", 75, 102, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100, 102, 1);
    tft.drawString("HIGH", 100, 102, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P03)), 100, 102, 1);

    tft.drawString("GPIO_04", 20, 116, 1);
    tft.drawString("->", 75, 116, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100, 116, 1);
    tft.drawString("HIGH", 100, 116, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P04)), 100, 116, 1);

    tft.drawString("GPIO_05", 20, 130, 1);
    tft.drawString("->", 75, 130, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100, 130, 1);
    tft.drawString("HIGH", 100, 130, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P05)), 100, 130, 1);

    tft.drawString("GPIO_06", 20, 144, 1);
    tft.drawString("->", 75, 144, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100, 144, 1);
    tft.drawString("HIGH", 100, 144, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P06)), 100, 144, 1);

    tft.drawString("GPIO_07", 20, 158, 1);
    tft.drawString("->", 75, 158, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100, 158, 1);
    tft.drawString("HIGH", 100, 158, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P07)), 100, 158, 1);

    tft.drawString("GPIO_08", 20 + 160, 60, 1);
    tft.drawString("->", 75 + 160, 60, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 60, 1);
    tft.drawString("HIGH", 100 + 160, 60, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P10)), 100 + 160, 60, 1);

    tft.drawString("GPIO_09", 20 + 160, 74, 1);
    tft.drawString("->", 75 + 160, 74, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 74, 1);
    tft.drawString("HIGH", 100 + 160, 74, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P11)), 100 + 160, 74, 1);

    tft.drawString("GPIO_10", 20 + 160, 88, 1);
    tft.drawString("->", 75 + 160, 88, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 88, 1);
    tft.drawString("HIGH", 100 + 160, 88, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P12)), 100 + 160, 88, 1);

    tft.drawString("GPIO_11", 20 + 160, 102, 1);
    tft.drawString("->", 75 + 160, 102, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 102, 1);
    tft.drawString("HIGH", 100 + 160, 102, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P13)), 100 + 160, 102, 1);

    tft.drawString("GPIO_12", 20 + 160, 116, 1);
    tft.drawString("->", 75 + 160, 116, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 116, 1);
    tft.drawString("HIGH", 100 + 160, 116, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P14)), 100 + 160, 116, 1);

    tft.drawString("GPIO_13", 20 + 160, 130, 1);
    tft.drawString("->", 75 + 160, 130, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 130, 1);
    tft.drawString("HIGH", 100 + 160, 130, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P15)), 100 + 160, 130, 1);

    tft.drawString("GPIO_14", 20 + 160, 144, 1);
    tft.drawString("->", 75 + 160, 144, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 144, 1);
    tft.drawString("HIGH", 100 + 160, 144, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P16)), 100 + 160, 144, 1);

    tft.drawString("GPIO_15", 20 + 160, 158, 1);
    tft.drawString("->", 75 + 160, 158, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 158, 1);
    tft.drawString("HIGH", 100 + 160, 158, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCA95x5_TFT_Show(PCA9535_Class.read(PCA95x5::Port::P17)), 100 + 160, 158, 1);
}

/**
 * @brief MPR121 test program for detecting whether touch IO is triggered and recording
the IO port number after the trigger for linkage debugging with PCA9535.

 * @return
 * @Date 2023-07-13 11:32:22
 */
void MPR121_Test(void)
{
    // delay(100);                         // Anti-jamming machine delay
    // touched1 = Touch_Sensor1.touched(); // Get touch data
    // touched2 = Touch_Sensor2.touched();

    for (uint8_t i = 0; i < 12; i++) {
        if (touched1 & (1 << i)) {
            PCA9535_Class.direction(PCA95x5::Direction::OUT_ALL);

            PCA9535_Class.write(PCA95x5::Level::H_ALL);

            PCA9535_Class.direction(PCA95x5::Direction::IN_ALL);
        } else if (touched2 & (1 << i)) {
            PCA9535_Class.direction(PCA95x5::Direction::OUT_ALL);

            PCA9535_Class.write(PCA95x5::Level::H_ALL);

            PCA9535_Class.direction(PCA95x5::Direction::IN_ALL);
        }
    }
}

/**
 * @brief Recursively fetch all files
 * @return
 * @Date 2023-07-13 11:36:10
 */
void getNextFile()
{
    String fileName;
    if (File_Scan_Flag) {
        File_Scan_Flag = false;
        File_Root = SD.open("/");
        if (!File_Root) {
            tft.println("Failed to open directory");
            tft.setCursor(0, 0);
            return;
        }
    }

    File_CurrentFile = File_Root.openNextFile();
    if (File_CurrentFile) {
        if (File_CurrentFile.isDirectory()) {
            tft.print("DIR: ");
            tft.println(File_CurrentFile.name());
            getNextFile(); // Recursively call to print all file names under the directory
        } else {
            fileName = File_CurrentFile.name();
            fileName.toLowerCase();
            tft.println("Filename: " + String(fileName));
            if (fileName.indexOf(".mp3") < 0) {
                getNextFile(); // If the .mp3 file is less than 3, continue looking for .mp3 file
            }
        }
    } else {
        File_Scan_Flag = true;
        getNextFile(); // Returns to the previous root directory until all file searches are complete
    }
}

/**
 * @brief Automatically select to play the next song
 * @return
 * @Date 2023-07-13 11:35:12
 */
void playNextAudio()
{
    String sdfile;
    getNextFile();
    if (File_CurrentFile) {
        sdfile = File_CurrentFile.name();
        audio.connecttoSD(sdfile.c_str());
        delay(50);
        tft.println("\nStart playing: " + sdfile);
        tft.setCursor(0, 0);
    }
}

void setup()
{
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    Serial.begin(115200);
    Serial.println("Ciallo T-Display-S3");

    tft.begin();
    tft.setRotation(3);
    // tft.setSwapBytes(true); // Make the picture color by RGB->BGRs
    tft.fillScreen(TFT_BLACK);

    ledcAttachPin(TFT_BL, 1); // assign TFT_BL pin to channel 1
    ledcSetup(1, 2000, 8);    // 2 kHz PWM, 8-bit resolution
    ledcWrite(1, 255);        // brightness 0 - 255
    attachInterrupt(2, PCA9535_MPR121_External_Interrupt, FALLING);

    tft.setTextColor(TFT_RED);
    tft.drawString("START", 0, 5, 4);
    tft.drawString("Press any MPR121 touch button to activate", 0, 80, 2);
    tft.setTextColor(TFT_BROWN);
    tft.drawString("|---TDS3_MIDI_Driver---|", 90, 10, 2);
    tft.drawString("LILYGO", 260, 5, 1);
    tft.drawString("V1.0.0", 260, 17, 1);
    tft.setTextColor(TFT_WHITE);

    // SD SPI
    SPI.begin(PIN_SD_CLK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS); // SPI boots

    Wire.begin(MPR121_SDA, MPR121_SCL);

    Touch_Sensor1.begin(MPR121_ADDR1, &Wire); // MPR121 address 1
    Touch_Sensor2.begin(MPR121_ADDR2, &Wire); // MPR121 address 2
    delay(50);

    PCA9535_Class.attach(Wire);
    PCA9535_Class.polarity(PCA95x5::Polarity::ORIGINAL_ALL);
    PCA9535_Class.direction(PCA95x5::Direction::OUT_ALL);
    // PCA9535_Class.write(PCA95x5::Level::H_ALL);
    PCA9535_Class.direction(PCA95x5::Direction::IN_ALL);
    delay(50);

    deviceScan(&Wire, &Serial);
    delay(50);

    // PCM5102A IIS
    audio.setPinout(PCM5102A_BCLK, PCM5102A_LRCLK, PCM5102A_DIN);
    audio.setVolume(10); // 0...21,Volume setting
}

void loop()
{
    // deviceScan(&Wire, &Serial);
    // delay(50);

    if (Windows == 0) {
        touched1 = Touch_Sensor1.touched(); // Get touch data
        touched2 = Touch_Sensor2.touched();
        if (MPR121_External_Interrupt_Flag == true) {
            MPR121_External_Interrupt_Flag = false;
            delay(100); // Anti-jamming machine delay

            if (touched2 & (1 << 11)) {
                delay(200); // Anti-jamming machine delay
                tft.fillScreen(TFT_BLACK);
                SD.end();
                delay(100); // Anti-jamming machine delay
                SD.begin(PIN_SD_CS, SPI, 40000000);
                delay(100); // Anti-jamming machine delay

                playNextAudio();
                Windows = !Windows;
            }
        }

        if (PCA95x5_External_Interrupt_Flag == true && Windows == 0) {
            PCA95x5_External_Interrupt_Flag = false;
            MPR121_Test();
            PCA95x5_Test();
        }
    }

    if (Windows == 1) {
        audio.loop();
    }
}
