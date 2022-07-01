/**
 * FunctionalButton.ino - Example for the OneButtonLibrary library.
 * This is a sample sketch to show how to use OneClick library functionally on ESP32,ESP8266... 
 * 
 */
#include <Arduino.h>
#include <OneButton.h>

class Button{
private:
  OneButton button;
  int value;
public:
  explicit Button(uint8_t pin):button(pin) {
    button.attachClick([](void *scope) { ((Button *) scope)->Clicked();}, this);
    button.attachDoubleClick([](void *scope) { ((Button *) scope)->DoubleClicked();}, this);
    button.attachLongPressStart([](void *scope) { ((Button *) scope)->LongPressed();}, this);
  }

  void Clicked(){
    Serial.println("Click then value++");
    value++;
  }

  void DoubleClicked(){

    Serial.println("DoubleClick");
  }

  void LongPressed(){
    Serial.println("LongPress and the value is");
    Serial.println(value);
  }

  void handle(){
    button.tick();
  }
};

Button button(0);

void setup() {
  Serial.begin(115200);
}

void loop() {
  button.handle();
}
