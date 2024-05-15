/**
 *
 * @license MIT License
 *
 * Copyright (c) 2022 lewis he
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      TouchDrvGT911.tpp
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2023-04-12
 *
 */
#pragma once

#include "REG/GT911Constants.h"
#include "TouchDrvInterface.hpp"
#include "SensorCommon.tpp"


typedef struct GT911_Struct {
    uint8_t trackID;
    int16_t x;
    int16_t y;
    int16_t size;
} GT911Point_t;

class TouchDrvGT911 :
    public TouchDrvInterface,
    public SensorCommon<TouchDrvGT911>
{
    friend class SensorCommon<TouchDrvGT911>;
public:


#if defined(ARDUINO)
    TouchDrvGT911(PLATFORM_WIRE_TYPE &w,
                  int sda = DEFAULT_SDA,
                  int scl = DEFAULT_SCL,
                  uint8_t addr = GT911_SLAVE_ADDRESS_H)
    {
        __wire = &w;
        __sda = sda;
        __scl = scl;
        __rst = SENSOR_PIN_NONE;
        __irq = SENSOR_PIN_NONE;
        __addr = addr;
    }
#endif

    TouchDrvGT911()
    {
#if defined(ARDUINO)
        __wire = &Wire;
        __sda = DEFAULT_SDA;
        __scl = DEFAULT_SCL;
#endif
        __rst = SENSOR_PIN_NONE;
        __irq = SENSOR_PIN_NONE;
        __addr = GT911_SLAVE_ADDRESS_H;
    }

    ~TouchDrvGT911()
    {
        deinit();
    }

#if defined(ARDUINO)
    bool begin(PLATFORM_WIRE_TYPE &w,
               uint8_t addr = GT911_SLAVE_ADDRESS_H,
               int sda = DEFAULT_SDA,
               int scl = DEFAULT_SCL
              )
    {
        return SensorCommon::begin(w, addr, sda, scl);
    }

#elif defined(ESP_PLATFORM) && !defined(ARDUINO)

#if ((ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5,0,0)) && defined(CONFIG_SENSORLIB_ESP_IDF_NEW_API))
    bool begin(i2c_master_bus_handle_t i2c_dev_bus_handle, uint8_t addr)
    {
        return SensorCommon::begin(i2c_dev_bus_handle, addr);
    }
#else
    bool begin(i2c_port_t port_num, uint8_t addr, int sda, int scl)
    {
        return SensorCommon::begin(port_num, addr, sda, scl);
    }
#endif //ESP_IDF_VERSION

#endif

    bool begin(uint8_t addr, iic_fptr_t readRegCallback, iic_fptr_t writeRegCallback)
    {
        return SensorCommon::begin(addr, readRegCallback, writeRegCallback);
    }


    void deinit()
    {
        // end();
    }


    void reset()
    {
        if (__rst != SENSOR_PIN_NONE) {
            this->setGpioMode(__rst, OUTPUT);
            this->setGpioLevel(__rst, HIGH);
            delay(10);
        }
        if (__irq != SENSOR_PIN_NONE) {
            this->setGpioMode(__irq, INPUT);
        }
        // writeRegister(GT911_COMMAND, 0x02);
        writeCommand(0x02);
    }

    void sleep()
    {
        if (__irq != SENSOR_PIN_NONE) {
            this->setGpioMode(__irq, OUTPUT);
            this->setGpioLevel(__irq, LOW);
        }
        // writeRegister(GT911_COMMAND, 0x05);
        writeCommand(0x05);

        /*
        * Depending on the chip and platform, setting it to input after removing sleep will affect power consumption. 
        * The chip platform determines whether
        * 
        * * */
        // if (__irq != SENSOR_PIN_NONE) {
        //     this->setGpioLevel(__irq, INPUT);
        // }
    }



    void wakeup()
    {
        if (__irq != SENSOR_PIN_NONE) {
            this->setGpioMode(__irq, OUTPUT);
            this->setGpioLevel(__irq, HIGH);
            delay(8);
        } else {
            reset();
        }
    }

    void idle()
    {

    }

    uint8_t getSupportTouchPoint()
    {
        return 5;
    }

    uint8_t getPoint(int16_t *x_array, int16_t *y_array, uint8_t size = 1)
    {
        uint8_t buffer[39];
        uint8_t touchPoint = 0;
        GT911Point_t p[5];

        if (!x_array || !y_array || size == 0)
            return 0;

        touchPoint = getPoint();
        if (touchPoint == 0) {
            return 0;
        }

        // GT911_POINT_1  0X814F
        uint8_t write_buffer[2] = {0x81, 0x4F};
        if (writeThenRead(write_buffer, SENSORLIB_COUNT(write_buffer),
                          buffer, 39) == DEV_WIRE_ERR) {
            return 0;
        }

        for (uint8_t i = 0; i < size; i++) {
            p[i].trackID = buffer[i * 8];
            p[i].x =  buffer[0x01 + i * 8] ;
            p[i].x |= (buffer[0x02 + i * 8] << 8 );
            p[i].y =  buffer[0x03 + i * 8] ;
            p[i].y |= (buffer[0x04 + i * 8] << 8);
            p[i].size = buffer[0x05 + i * 8] ;
            p[i].size |= (buffer[0x06 + i * 8] << 8) ;

            x_array[i] = p[i].x;
            y_array[i] = p[i].y;
        }

#ifdef LOG_PORT
        LOG_PORT.println("---------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
        LOG_PORT.println("Touched  [0]ID  [0]Size  [0]X   [0]Y  [1]ID  [1]Size   [1]X    [1]Y  [2]ID  [2]Size   [2]X    [2]Y  [3]ID  [3]Size  [3]X    [3]Y  [4]ID  [4]Size  [4]X    [4]Y  ");
        LOG_PORT.print(touchPoint); LOG_PORT.print("\t");
        for (int i = 0; i < size; ++i) {
            LOG_PORT.print(p[i].trackID); LOG_PORT.print("\t");
            LOG_PORT.print(p[i].size); LOG_PORT.print("\t");
            LOG_PORT.print( p[i].x); LOG_PORT.print("\t");
            LOG_PORT.print( p[i].y); LOG_PORT.print("\t");
        }
        LOG_PORT.println();
#endif

        updateXY(touchPoint, x_array, y_array);

        return touchPoint;
    }


    bool isPressed()
    {
        if (__irq != SENSOR_PIN_NONE) {
            if (__irq_mode == FALLING) {
                return this->getGpioLevel(__irq) == LOW;
            } else if (__irq_mode == RISING ) {
                return this->getGpioLevel(__irq) == HIGH;
            }
        } else {
            return getPoint();
        }
        return false;
    }

    //In the tested GT911 only the falling edge is valid to use, the rest are incorrect
    bool setInterruptMode(uint8_t mode)
    {
        // GT911_MODULE_SWITCH_1 0x804D
        uint8_t val = readGT911(GT911_MODULE_SWITCH_1);
        val &= 0XFC;
        if (mode == FALLING) {
            val |= 0x03;
        } else if (mode == RISING ) {
            val |= 0x02;
        }
        __irq_mode = mode;
        return writeGT911(GT911_MODULE_SWITCH_1, val) != DEV_WIRE_ERR;
    }


    uint8_t getPoint()
    {
        // GT911_POINT_INFO 0X814E
        uint8_t val = readGT911(GT911_POINT_INFO) & 0x0F;
        clearBuffer();
        return val & 0x0F;
    }


    uint32_t getChipID()
    {
        char product_id[4] = {0};
        // GT911_PRODUCT_ID 0x8140
        for (int i = 0; i < 4; ++i) {
            product_id[i] = readGT911(GT911_PRODUCT_ID + i);
        }
        return atoi(product_id);
    }

    uint16_t getFwVersion()
    {
        uint8_t fw_ver[2] = {0};
        // GT911_FIRMWARE_VERSION 0x8144
        for (int i = 0; i < 2; ++i) {
            fw_ver[i] = readGT911(GT911_FIRMWARE_VERSION + i);
        }
        return fw_ver[0] | (fw_ver[1] << 8);
    }

    bool getResolution(int16_t *x, int16_t *y)
    {
        uint8_t x_resolution[2] = {0}, y_resolution[2] = {0};

        for (int i = 0; i < 2; ++i) {
            x_resolution[i] = readGT911(GT911_X_RESOLUTION + i);
        }
        for (int i = 0; i < 2; ++i) {
            y_resolution[i] = readGT911(GT911_Y_RESOLUTION + i);
        }

        *x = x_resolution[0] | (x_resolution[1] << 8);
        *y = y_resolution[0] | (y_resolution[1] << 8);
        return true;
    }

    int getVendorID()
    {
        return readGT911(GT911_VENDOR_ID);
    }


    const char *getModelName()
    {
        return "GT911";
    }

    void  setGpioCallback(gpio_mode_fptr_t mode_cb,
                          gpio_write_fptr_t write_cb,
                          gpio_read_fptr_t read_cb)
    {
        SensorCommon::setGpioModeCallback(mode_cb);
        SensorCommon::setGpioWriteCallback(write_cb);
        SensorCommon::setGpioReadCallback(read_cb);
    }

private:

    uint8_t readGT911(uint16_t cmd)
    {
        uint8_t value = 0x00;
        uint8_t write_buffer[2] = {highByte(cmd), lowByte(cmd)};
        writeThenRead(write_buffer, SENSORLIB_COUNT(write_buffer),
                      &value, 1);
        return value;
    }

    int writeGT911(uint16_t cmd, uint8_t value)
    {
        uint8_t write_buffer[3] = {highByte(cmd), lowByte(cmd), value};
        return writeBuffer(write_buffer, SENSORLIB_COUNT(write_buffer));
    }


    void writeCommand(uint8_t command)
    {
        // GT911_COMMAND 0x8040
        uint8_t write_buffer[3] = {0x80, 0x40, command};
        writeBuffer(write_buffer, SENSORLIB_COUNT(write_buffer));
    }

    void inline clearBuffer()
    {
        writeGT911(GT911_POINT_INFO, 0x00);
    }

    bool initImpl()
    {
        int16_t x = 0, y = 0;

        if (__addr == GT911_SLAVE_ADDRESS_H  &&
                __rst != SENSOR_PIN_NONE &&
                __irq != SENSOR_PIN_NONE) {

            log_i("GT911 using 0x28 address!\n");

            this->setGpioMode(__rst, OUTPUT);
            this->setGpioMode(__irq, OUTPUT);

            this->setGpioLevel(__rst, LOW);
            this->setGpioLevel(__irq, HIGH);
            delayMicroseconds(120);
            this->setGpioLevel(__rst, HIGH);
            delay(18);
            this->setGpioMode(__irq, INPUT);

        } else if (__addr == GT911_SLAVE_ADDRESS_L &&
                   __rst != SENSOR_PIN_NONE &&
                   __irq != SENSOR_PIN_NONE) {

            log_i("GT911 using 0xBA address!\n");

            this->setGpioMode(__rst, OUTPUT);
            this->setGpioMode(__irq, OUTPUT);

            this->setGpioLevel(__rst, LOW);
            this->setGpioLevel(__irq, LOW);
            delayMicroseconds(120);
            this->setGpioLevel(__rst, HIGH);
            delay(18);
            this->setGpioMode(__irq, INPUT);

        } else {
            reset();
        }

        // For variants where the GPIO is controlled by I2C, a delay is required here
        delay(20);

        __chipID = getChipID();
        log_i("Product id:%ld\n", __chipID);

        if (__chipID != 911) {
            log_i("Not find device GT911\n");
            return false;
        }
        log_i("Firmware version: 0x%x\n", getFwVersion());
        getResolution(&x, &y);
        log_i("Resolution : X = %d Y = %d\n", x, y);
        log_i("Vendor id:%d\n", getVendorID());
        return true;
    }

    int getReadMaskImpl()
    {
        return -1;
    }


protected:
    int __irq_mode;
};



