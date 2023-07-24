#include <PCA95x5.h>

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
