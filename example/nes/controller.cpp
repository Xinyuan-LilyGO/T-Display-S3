#include "pin_config.h"
#include <Arduino.h>
/* controller is GPIO */
#if defined(HW_CONTROLLER_GPIO)

extern "C" void controller_init() {
#if defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK)
  pinMode(HW_CONTROLLER_GPIO_UP_DOWN, INPUT);
  pinMode(HW_CONTROLLER_GPIO_LEFT_RIGHT, INPUT);
#else  /* !defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK) */
  pinMode(HW_CONTROLLER_GPIO_UP, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_DOWN, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_LEFT, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_RIGHT, INPUT_PULLUP);
#endif /* !defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK) */
  pinMode(HW_CONTROLLER_GPIO_SELECT, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_START, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_A, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_B, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_X, INPUT_PULLUP);
  pinMode(HW_CONTROLLER_GPIO_Y, INPUT_PULLUP);
}

extern "C" uint32_t controller_read_input() {
  uint32_t u, d, l, r, s, t, a, b, x, y;

#if defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK)

#if defined(HW_CONTROLLER_GPIO_REVERSE_UP)
  int joyY = 4095 - analogRead(HW_CONTROLLER_GPIO_UP_DOWN);
#else  /* !defined(HW_CONTROLLER_GPIO_REVERSE_UD) */
  int joyY = analogRead(HW_CONTROLLER_GPIO_UP_DOWN);
#endif /* !defined(HW_CONTROLLER_GPIO_REVERSE_UD) */

#if defined(HW_CONTROLLER_GPIO_REVERSE_LF)
  int joyX = 4095 - analogRead(HW_CONTROLLER_GPIO_LEFT_RIGHT);
#else  /* !defined(HW_CONTROLLER_GPIO_REVERSE_LF) */
  int joyX = analogRead(HW_CONTROLLER_GPIO_LEFT_RIGHT);
#endif /* !defined(HW_CONTROLLER_GPIO_REVERSE_LF) */

  if (joyY > 2048 + 1024) {
    u = 1;
    d = 0;
  } else if (joyY < 1024) {
    u = 0;
    d = 1;
  } else {
    u = 1;
    d = 1;
  }

  if (joyX > 2048 + 1024) {
    l = 1;
    r = 0;
  } else if (joyX < 1024) {
    l = 0;
    r = 1;
  } else {
    l = 1;
    r = 1;
  }

#else  /* !defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK) */
  u = digitalRead(HW_CONTROLLER_GPIO_UP);
  d = digitalRead(HW_CONTROLLER_GPIO_DOWN);
  l = digitalRead(HW_CONTROLLER_GPIO_LEFT);
  r = digitalRead(HW_CONTROLLER_GPIO_RIGHT);
#endif /* !defined(HW_CONTROLLER_GPIO_ANALOG_JOYSTICK) */

  s = digitalRead(HW_CONTROLLER_GPIO_SELECT);
  t = digitalRead(HW_CONTROLLER_GPIO_START);
  a = digitalRead(HW_CONTROLLER_GPIO_A);
  b = digitalRead(HW_CONTROLLER_GPIO_B);
  x = digitalRead(HW_CONTROLLER_GPIO_X);
  y = digitalRead(HW_CONTROLLER_GPIO_Y);
  return 0xFFFFFFFF ^ ((!u << 0) | (!d << 1) | (!l << 2) | (!r << 3) | (!s << 4) | (!t << 5) | (!a << 6) | (!b << 7) | (!x << 8) | (!y << 9));
}

#elif defined(HW_CONTROLLER_DABBLE_APP)

#include <DabbleESP32.h>

extern "C" void controller_init() { Dabble.begin("t-Display-S3-NES"); }

extern "C" uint32_t controller_read_input() {
  uint32_t u, d, l, r, s, t, a, b, x, y;

  Dabble.processInput();

  d = GamePad.isUpPressed();
  u = GamePad.isDownPressed();
  r = GamePad.isLeftPressed();
  l = GamePad.isRightPressed();

  s = !GamePad.isSelectPressed();
  t = !GamePad.isStartPressed();
  y = !GamePad.isTrianglePressed();
  b = !GamePad.isCrossPressed();
  x = !GamePad.isSquarePressed();
  a = !GamePad.isCirclePressed();

  return 0xFFFFFFFF ^ ((!u << 0) | (!d << 1) | (!l << 2) | (!r << 3) | (!s << 4) | (!t << 5) | (!a << 6) | (!b << 7) | (!x << 8) | (!y << 9));
}

#else /* no controller defined */

extern "C" void controller_init() { Serial.printf("GPIO controller disabled in menuconfig; no input enabled.\n"); }

extern "C" uint32_t controller_read_input() { return 0xFFFFFFFF; }

#endif /* no controller defined */
