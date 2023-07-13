/*
 * @Description: This program is used to test the T-Display-S3-MIDI program and consists
of 3 parts, 1.PCF8575_Test, 2.MPR121_Test, 3.PCM5102A_Test

    When the program is freshly burned, you can see the "START" text in red. By touching
any of the TOUCH0 (0-11) or TOUCH1 (0-10) pins of the MPR121 touch sensor, the system is
activated. At this point, the "START" text changes to purple, indicating that the PCF8575
and MPR121 modes are being tested. Next, when you touch the touch IO pins, you can
observe that the GPIO level of PCF8575 on the screen flips (one flip per touch). The touch
range includes TOUCH0 (0-11) and TOUCH1 (0-3). If this occurs, it indicates a successful
test of PCF8575 and MPR121; otherwise, it fails.
    When touching the 11th pin of TOUCH1, the system can be switched to test PCM5102A
mode.If an SD card is present, it will scan all files inside and display them on the T-Display-S3
screen. It will then proceed to play the ".mp3" file. Otherwise, the screen will display
"Failed to open directory" (requiring a mode switch to attempt playback again). Any other
situation will be considered a test failure.

 * @version: V1.0.0
 * @Author: LILYGO_L
 * @Date: 2023-06-12 16:16:37
 * @LastEditors: LILYGO_L
 * @LastEditTime: 2023-07-10 09:56:52
 */

#include "Arduino.h"
#include "PCF8575.h"
#include "TFT_eSPI.h"
#include "pin_config.h"
#include "Adafruit_MPR121.h"
#include "FS.h"
#include "SPI.h"
#include "SD.h"
#include "Audio.h"

TFT_eSPI tft;
Audio audio;

int32_t Windows = 0;

// Function interrupt
void PCF8575_MPR121_External_Interrupt();

// Set IIC address
PCF8575 PCF8575_Class(T_DISPLAY_PCF8575_IIC_ADDRESS, PCF8575_SDA, PCF8575_SCL, T_DISPLAY_PCF8575_INTERRUPTPIN, PCF8575_MPR121_External_Interrupt);
bool PCF8575_External_Interrupt_Flag = false;

Adafruit_MPR121 Touch_Sensor1 = Adafruit_MPR121();
Adafruit_MPR121 Touch_Sensor2 = Adafruit_MPR121();
bool MPR121_External_Interrupt_Flag = false;

uint16_t touched1 = 0;
uint16_t touched2 = 0;
uint16_t MPR121_IRQ_Number = 0;

File File_CurrentFile;
File File_Root;
bool File_Scan_Flag = true;

void PCF8575_MPR121_External_Interrupt()
{
    // Interrupt called (No Serial no read no wire in this function, and DEBUG disabled on PCF library)
    PCF8575_External_Interrupt_Flag = true;
    MPR121_External_Interrupt_Flag = true;
}

const char *PCF8575_TFT_Show(uint8_t Num)
{
    if (Num == 0)
    {
        return "LOW";
    }
    else
    {
        return "HIGH";
    }
}

/**
 * @brief PCF8575 test program, detect the mode of IO ports, and measure the voltage
level of the corresponding IO ports using a multimeter.

 * @return
 * @Date 2023-07-13 10:06:19
 */
void PCF8575_Test(void)
{
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_VIOLET);
    tft.drawString("START", 0, 5, 4);
    tft.setTextColor(TFT_BROWN);
    // tft.drawString("|---PCF8575_MPR121---|", 100, 10, 2);
    tft.drawString("|---TDS3_MIDI_Driver---|", 90, 10, 2);
    tft.drawString("LILYGO", 260, 5, 1);
    tft.drawString("V1.0.0", 260, 17, 1);
    tft.setTextColor(TFT_WHITE);

    PCF8575::DigitalInput readAllPin = PCF8575_Class.digitalReadAll();
    delay(50); // Anti-jamming machine delay

    tft.drawString("READ VALUE FROM PCF8575: ", 5, 30, 2);

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
    tft.drawString(PCF8575_TFT_Show(readAllPin.p0), 100, 60, 1);

    tft.drawString("GPIO_01", 20, 74, 1);
    tft.drawString("->", 75, 74, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100, 74, 1);
    tft.drawString("HIGH", 100, 74, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCF8575_TFT_Show(readAllPin.p1), 100, 74, 1);

    tft.drawString("GPIO_02", 20, 88, 1);
    tft.drawString("->", 75, 88, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100, 88, 1);
    tft.drawString("HIGH", 100, 88, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCF8575_TFT_Show(readAllPin.p2), 100, 88, 1);

    tft.drawString("GPIO_03", 20, 102, 1);
    tft.drawString("->", 75, 102, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100, 102, 1);
    tft.drawString("HIGH", 100, 102, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCF8575_TFT_Show(readAllPin.p3), 100, 102, 1);

    tft.drawString("GPIO_04", 20, 116, 1);
    tft.drawString("->", 75, 116, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100, 116, 1);
    tft.drawString("HIGH", 100, 116, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCF8575_TFT_Show(readAllPin.p4), 100, 116, 1);

    tft.drawString("GPIO_05", 20, 130, 1);
    tft.drawString("->", 75, 130, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100, 130, 1);
    tft.drawString("HIGH", 100, 130, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCF8575_TFT_Show(readAllPin.p5), 100, 130, 1);

    tft.drawString("GPIO_06", 20, 144, 1);
    tft.drawString("->", 75, 144, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100, 144, 1);
    tft.drawString("HIGH", 100, 144, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCF8575_TFT_Show(readAllPin.p6), 100, 144, 1);

    tft.drawString("GPIO_07", 20, 158, 1);
    tft.drawString("->", 75, 158, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100, 158, 1);
    tft.drawString("HIGH", 100, 158, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCF8575_TFT_Show(readAllPin.p7), 100, 158, 1);

    tft.drawString("GPIO_08", 20 + 160, 60, 1);
    tft.drawString("->", 75 + 160, 60, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 60, 1);
    tft.drawString("HIGH", 100 + 160, 60, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCF8575_TFT_Show(readAllPin.p8), 100 + 160, 60, 1);

    tft.drawString("GPIO_09", 20 + 160, 74, 1);
    tft.drawString("->", 75 + 160, 74, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 74, 1);
    tft.drawString("HIGH", 100 + 160, 74, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCF8575_TFT_Show(readAllPin.p9), 100 + 160, 74, 1);

    tft.drawString("GPIO_10", 20 + 160, 88, 1);
    tft.drawString("->", 75 + 160, 88, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 88, 1);
    tft.drawString("HIGH", 100 + 160, 88, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCF8575_TFT_Show(readAllPin.p10), 100 + 160, 88, 1);

    tft.drawString("GPIO_11", 20 + 160, 102, 1);
    tft.drawString("->", 75 + 160, 102, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 102, 1);
    tft.drawString("HIGH", 100 + 160, 102, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCF8575_TFT_Show(readAllPin.p11), 100 + 160, 102, 1);

    tft.drawString("GPIO_12", 20 + 160, 116, 1);
    tft.drawString("->", 75 + 160, 116, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 116, 1);
    tft.drawString("HIGH", 100 + 160, 116, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCF8575_TFT_Show(readAllPin.p12), 100 + 160, 116, 1);

    tft.drawString("GPIO_13", 20 + 160, 130, 1);
    tft.drawString("->", 75 + 160, 130, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 130, 1);
    tft.drawString("HIGH", 100 + 160, 130, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCF8575_TFT_Show(readAllPin.p13), 100 + 160, 130, 1);

    tft.drawString("GPIO_14", 20 + 160, 144, 1);
    tft.drawString("->", 75 + 160, 144, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 144, 1);
    tft.drawString("HIGH", 100 + 160, 144, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCF8575_TFT_Show(readAllPin.p14), 100 + 160, 144, 1);

    tft.drawString("GPIO_15", 20 + 160, 158, 1);
    tft.drawString("->", 75 + 160, 158, 1);
    tft.setTextColor(TFT_BLACK);
    tft.drawString("LOW", 100 + 160, 158, 1);
    tft.drawString("HIGH", 100 + 160, 158, 1);
    tft.setTextColor(TFT_WHITE);
    tft.drawString(PCF8575_TFT_Show(readAllPin.p15), 100 + 160, 158, 1);
}

/**
 * @brief MPR121 test program for detecting whether touch IO is triggered and recording
the IO port number after the trigger for linkage debugging with PCF8575.

 * @return
 * @Date 2023-07-13 11:32:22
 */
void MPR121_Test(void)
{
    for (uint8_t i = 0; i < 12; i++)
    {
        if (touched1 & (1 << i))
        {
            for (uint8_t i = 0; i < 16; i++)
            {
                PCF8575_Class.pinMode(i, OUTPUT); // Write to output
            }
            PCF8575_Class.digitalWrite(i, !PCF8575_Class.digitalRead(i));
            for (uint8_t i = 0; i < 16; i++)
            {
                PCF8575_Class.pinMode(i, INPUT);
            }
        }
        else if (touched2 & (1 << i))
        {
            for (uint8_t i = 0; i < 16; i++)
            {
                PCF8575_Class.pinMode(i, OUTPUT);
            }
            PCF8575_Class.digitalWrite(i + 12, !PCF8575_Class.digitalRead(i + 12));
            for (uint8_t i = 0; i < 16; i++)
            {
                PCF8575_Class.pinMode(i, INPUT);
            }
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
    if (File_Scan_Flag)
    {
        File_Scan_Flag = false;
        File_Root = SD.open("/");
        if (!File_Root)
        {
            tft.println("Failed to open directory");
            tft.setCursor(0, 0);
            return;
        }
    }

    File_CurrentFile = File_Root.openNextFile();
    if (File_CurrentFile)
    {
        if (File_CurrentFile.isDirectory())
        {
            tft.print("DIR: ");
            tft.println(File_CurrentFile.name());
            getNextFile(); // Recursively call to print all file names under the directory
        }
        else
        {
            fileName = File_CurrentFile.name();
            fileName.toLowerCase();
            tft.println("Filename: " + String(fileName));
            if (fileName.indexOf(".mp3") < 0)
            {
                getNextFile(); // If the .mp3 file is less than 3, continue looking for .mp3 file
            }
        }
    }
    else
    {
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
    if (File_CurrentFile)
    {
        sdfile = File_CurrentFile.name();
        audio.connecttoSD(sdfile.c_str());
        tft.println("\nStart playing: " + sdfile);
        tft.setCursor(0, 0);
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Hello T-Display-S3");

    tft.begin();
    tft.setRotation(3);
    // tft.setSwapBytes(true); // Make the picture color by RGB->BGRs
    tft.fillScreen(TFT_BLACK);

    ledcAttachPin(TFT_BL, 1); // assign TFT_BL pin to channel 1
    ledcSetup(1, 12000, 8);   // 12 kHz PWM, 8-bit resolution
    ledcWrite(1, 255);        // brightness 0 - 255

    // SD SPI
    SPI.begin(PIN_SD_CLK, PIN_SD_MISO, PIN_SD_MOSI, PIN_SD_CS); // SPI boots

    PCF8575_Class.begin();
    for (uint8_t i = 0; i < 16; i++)
    {
        PCF8575_Class.pinMode(i, INPUT); // Set the IO mode for all PCF8575s
    }

    Touch_Sensor1.begin(MPR121_ADDR1); // MPR121 address 1
    Touch_Sensor2.begin(MPR121_ADDR2); // MPR121 address 2

    // PCM5102A IIS
    audio.setPinout(PCM5102A_BCK, PCM5102A_LRCK, PCM5102A_DIN);
    audio.setVolume(10); // 0...21,Volume setting

    tft.setTextColor(TFT_RED);
    tft.drawString("START", 0, 5, 4);
    tft.drawString("Press any MPR121 touch button to activate", 0, 80, 2);
    tft.setTextColor(TFT_BROWN);
    tft.drawString("|---TDS3_MIDI_Driver---|", 90, 10, 2);
    tft.drawString("LILYGO", 260, 5, 1);
    tft.drawString("V1.0.0", 260, 17, 1);
    tft.setTextColor(TFT_WHITE);
}

void loop()
{
    if (MPR121_External_Interrupt_Flag == true)
    {
        MPR121_External_Interrupt_Flag = false;

        delay(100);                         // Anti-jamming machine delay
        touched1 = Touch_Sensor1.touched(); // Get touch data
        touched2 = Touch_Sensor2.touched();

        if (touched2 & (1 << 11))
        {
            tft.fillScreen(TFT_BLACK);
            SD.end();
            SD.begin(PIN_SD_CS, SPI, 40000000);

            playNextAudio();
            Windows = !Windows;
        }
        else
        {
            MPR121_Test();
        }
    }

    if (PCF8575_External_Interrupt_Flag == true && Windows == 0)
    {
        PCF8575_External_Interrupt_Flag = false;
        PCF8575_Test();
    }

    if (Windows == 1)
    {
        audio.loop();
    }
}
