# PCA95x5

Arduino library for [PCA9535](https://www.ti.com/product/PCA9535) and [PCA9555](https://www.ti.com/product/PCA9555) (Remote 16-bit I2C and SMBus I/O Expander with Interrupt Output and Configuration Registers).

## Usage

You can specify a port using either index (`0` - `15`) or enum (`PCA95x5::Port::P02`, etc.).

### Input

```C++
#include <PCA95x5.h>

// select one of them
PCA9535 ioex;
// PCA9555 ioex;

void setup() {
    Serial.begin(115200);
    delay(2000);

    Wire.begin();
    ioex.attach(Wire);
    ioex.polarity(PCA95x5::Polarity::ORIGINAL_ALL);
    ioex.direction(PCA95x5::Direction::IN_ALL);
}

void loop() {
    Serial.println(ioex.read(), BIN);
    delay(1000);
}
```

### Output

```C++
#include <PCA95x5.h>

// select one of them
// PCA9535 ioex;
PCA9555 ioex;

void setup() {
    Serial.begin(115200);
    delay(2000);

    Wire.begin();
    ioex.attach(Wire);
    ioex.polarity(PCA95x5::Polarity::ORIGINAL_ALL);
    ioex.direction(PCA95x5::Direction::OUT_ALL);
    ioex.write(PCA95x5::Level::L_ALL);
}

void loop() {
    for (size_t i = 0; i < 16; ++i) {
        Serial.print("set port high: ");
        Serial.println(i);

        ioex.write(i, PCA95x5::Level::H);
        Serial.println(ioex.read(), BIN);
        delay(500);
    }

    for (size_t i = 0; i < 16; ++i) {
        Serial.print("set port low: ");
        Serial.println(i);

        ioex.write(i, PCA95x5::Level::L);
        Serial.println(ioex.read(), BIN);
        delay(500);
    }
}
```

## APIs

```C++
void attach(WireType& wire, uint8_t i2c_addr = BASE_I2C_ADDR);
uint16_t read();
Level::Level read(const uint8_t port);
bool write(const uint16_t value);
bool write(const uint8_t port, const Level::Level level);
bool polarity(const uint16_t value);
bool polarity(const uint8_t port, const Polarity::Polarity pol);
bool direction(const uint16_t value);
bool direction(const uint8_t port, const Direction::Direction dir);
uint8_t i2c_error() const;
```

```C++
namespace Port {
    enum Port : uint8_t {
        P00,
        P01,
        P02,
        P03,
        P04,
        P05,
        P06,
        P07,
        P10,
        P11,
        P12,
        P13,
        P14,
        P15,
        P16,
        P17,
    };
}
namespace Level {
    enum Level : uint8_t { L, H };
    enum LevelAll : uint16_t { L_ALL = 0x0000, H_ALL = 0xFFFF };
}
namespace Polarity {
    enum Polarity : uint8_t { ORIGINAL, INVERTED };
    enum PolarityAll : uint16_t { ORIGINAL_ALL = 0x0000, INVERTED_ALL = 0xFFFF };
}
namespace Direction {
    enum Direction : uint8_t { OUT, IN };
    enum DirectionAll : uint16_t { OUT_ALL = 0x0000, IN_ALL = 0xFFFF };
}
```

## License

MIT
