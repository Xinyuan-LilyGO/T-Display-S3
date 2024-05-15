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
 * @file      ExtensionIOXL9555.hpp
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2022-12-27
 *
 */
#pragma once

#include "REG/XL9555Constants.h"
#include "SensorCommon.tpp"
#include "ExtensionSPI.tpp"

class ExtensionIOXL9555 :
    public SensorCommon<ExtensionIOXL9555>,
    public ExtensionSPI<ExtensionIOXL9555>
{
    friend class SensorCommon<ExtensionIOXL9555>;
    friend class ExtensionSPI<ExtensionIOXL9555>;
public:

    enum {
        PORT0,
        PORT1,
    };

    enum ExtensionGPIO {
        IO0,
        IO1,
        IO2,
        IO3,
        IO4,
        IO5,
        IO6,
        IO7,
        IO8,
        IO9,
        IO10,
        IO11,
        IO12,
        IO13,
        IO14,
        IO15,
    };


#if defined(ARDUINO)
    ExtensionIOXL9555(PLATFORM_WIRE_TYPE &w, int sda = DEFAULT_SDA, int scl = DEFAULT_SCL, uint8_t addr = XL9555_SLAVE_ADDRESS0)
    {
        __wire = &w;
        __sda = sda;
        __scl = scl;
        __addr = addr;
    }
#endif

    ExtensionIOXL9555()
    {
#if defined(ARDUINO)
        __wire = &Wire;
        __sda = DEFAULT_SDA;
        __scl = DEFAULT_SCL;
#endif
        __addr = XL9555_SLAVE_ADDRESS0;
    }

    ~ExtensionIOXL9555()
    {
        deinit();
    }

#if defined(ARDUINO)
    bool init(PLATFORM_WIRE_TYPE &w, int sda = DEFAULT_SDA, int scl = DEFAULT_SCL, uint8_t addr = XL9555_SLAVE_ADDRESS0)
    {
        return SensorCommon::begin(w, addr, sda, scl);
    }
#endif


    void deinit()
    {
        // end();
    }


    void pinMode(uint8_t pin, uint8_t mode)
    {
        uint8_t registers = 0;
        if (pin >= IO8) {
            pin -= IO8;
            registers = XL9555_CTRL_CFG1;
        } else {
            registers = XL9555_CTRL_CFG0;
        }
        switch (mode) {
        case INPUT:
            setRegisterBit(registers, pin);
            break;
        case OUTPUT:
            clrRegisterBit(registers, pin);
            break;
        default:
            break;
        }
    }

    int digitalRead(uint8_t pin)
    {
        uint8_t registers = 0;
        if (pin >= IO8) {
            pin -= IO8;
            registers = XL9555_CTRL_INP1;
        } else {
            registers = XL9555_CTRL_INP0;
        }
        return getRegisterBit(registers, pin);
    }

    void digitalWrite(uint8_t pin, uint8_t val)
    {
        uint8_t registers = 0;
        if (pin >= IO8) {
            pin -= IO8;
            registers = XL9555_CTRL_OUTP1;
        } else {
            registers = XL9555_CTRL_OUTP0;
        }
        val ? setRegisterBit(registers, pin) : clrRegisterBit(registers,  pin);
    }

    void digitalToggle(uint8_t pin)
    {
        int state = 1 - digitalRead(pin);
        digitalWrite(pin, state);
    }

    int readPort(uint8_t port)
    {
        return readRegister(port == PORT0 ? XL9555_CTRL_INP0 : XL9555_CTRL_INP1);
    }

    int writePort(uint8_t port, uint8_t mask)
    {
        return writeRegister(port == PORT0 ? XL9555_CTRL_OUTP0 : XL9555_CTRL_OUTP1, mask);
    }

    int readConfig(uint8_t port)
    {
        return readRegister(port == PORT0 ? XL9555_CTRL_CFG0 : XL9555_CTRL_CFG1);
    }

    int configPort(uint8_t port, uint8_t mask)
    {
        return writeRegister(port == PORT0 ? XL9555_CTRL_CFG0 : XL9555_CTRL_CFG1, mask);
    }


private:
    bool initImpl()
    {
        int cfg1 = readConfig(PORT0);
        if (cfg1 == DEV_WIRE_ERR)return false;
        configPort(PORT0, 0x03);
        uint8_t val = readConfig(PORT0);
        if (val != 0x03) {
            return false;
        }
        writePort(PORT0, cfg1);
        return true;
    }

    int getReadMaskImpl()
    {
        return -1;
    }

protected:

};



