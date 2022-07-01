// -----
// OneButton.h - Library for detecting button clicks, doubleclicks and long
// press pattern on a single button. This class is implemented for use with the
// Arduino environment. Copyright (c) by Matthias Hertel,
// http://www.mathertel.de This work is licensed under a BSD style license. See
// http://www.mathertel.de/License.aspx More information on:
// http://www.mathertel.de/Arduino
// -----
// 02.10.2010 created by Matthias Hertel
// 21.04.2011 transformed into a library
// 01.12.2011 include file changed to work with the Arduino 1.0 environment
// 23.03.2014 Enhanced long press functionalities by adding longPressStart and
// longPressStop callbacks
// 21.09.2015 A simple way for debounce detection added.
// 14.05.2017 Debouncing improvements.
// 25.06.2018 Optional third parameter for deactivating pullup.
// 26.09.2018 Anatoli Arkhipenko: Included solution to use library with other
// sources of input.
// 26.09.2018 Initialization moved into class declaration.
// 26.09.2018 Jay M Ericsson: compiler warnings removed.
// 29.01.2020 improvements from ShaggyDog18
// -----

#ifndef OneButton_h
#define OneButton_h

#include "Arduino.h"

// ----- Callback function types -----

extern "C" {
typedef void (*callbackFunction)(void);
typedef void (*parameterizedCallbackFunction)(void *);
}


class OneButton
{
public:
  // ----- Constructor -----
  OneButton();

  /**
   * Initialize the OneButton library.
   * @param pin The pin to be used for input from a momentary button.
   * @param activeLow Set to true when the input level is LOW when the button is pressed, Default is true.
   * @param pullupActive Activate the internal pullup when available. Default is true.
   */
  OneButton(const int pin, const boolean activeLow = true, const bool pullupActive = true);

  // ----- Set runtime parameters -----

  /**
   * set # millisec after safe click is assumed.
   */
  void setDebounceTicks(const int ticks);

  /**
   * set # millisec after single click is assumed.
   */
  void setClickTicks(const int ticks);

  /**
   * set # millisec after press is assumed.
   */
  void setPressTicks(const int ticks);

  /**
   * Attach an event to be called when a single click is detected.
   * @param newFunction This function will be called when the event has been detected.
   */
  void attachClick(callbackFunction newFunction);
  void attachClick(parameterizedCallbackFunction newFunction, void *parameter);

  /**
   * Attach an event to be called after a double click is detected.
   * @param newFunction This function will be called when the event has been detected.
   */
  void attachDoubleClick(callbackFunction newFunction);
  void attachDoubleClick(parameterizedCallbackFunction newFunction, void *parameter);

  /**
   * Attach an event to be called after a multi click is detected.
   * @param newFunction This function will be called when the event has been detected.
   */
  void attachMultiClick(callbackFunction newFunction);
  void attachMultiClick(parameterizedCallbackFunction newFunction, void *parameter);

  /**
   * Attach an event to fire when the button is pressed and held down.
   * @param newFunction
   */
  void attachLongPressStart(callbackFunction newFunction);
  void attachLongPressStart(parameterizedCallbackFunction newFunction, void *parameter);

  /**
   * Attach an event to fire as soon as the button is released after a long press.
   * @param newFunction
   */
  void attachLongPressStop(callbackFunction newFunction);
  void attachLongPressStop(parameterizedCallbackFunction newFunction, void *parameter);

  /**
   * Attach an event to fire periodically while the button is held down.
   * @param newFunction
   */
  void attachDuringLongPress(callbackFunction newFunction);
  void attachDuringLongPress(parameterizedCallbackFunction newFunction, void *parameter);

  // ----- State machine functions -----

  /**
   * @brief Call this function every some milliseconds for checking the input
   * level at the initialized digital pin.
   */
  void tick(void);


  /**
   * @brief Call this function every time the input level has changed.
   * Using this function no digital input pin is checked because the current
   * level is given by the parameter.
   */
  void tick(bool level);


  /**
   * Reset the button state machine.
   */
  void reset(void);


  /*
   * return number of clicks in any case: single or multiple clicks
   */
  int getNumberClicks(void);


  /**
   * @return true if we are currently handling button press flow
   * (This allows power sensitive applications to know when it is safe to power down the main CPU)
   */
  bool isIdle() const { return _state == OCS_INIT; }

  /**
   * @return true when a long press is detected
   */
  bool isLongPressed() const { return _state == OCS_PRESS; };


private:
  int _pin;                         // hardware pin number.
  unsigned int _debounceTicks = 50; // number of ticks for debounce times.
  unsigned int _clickTicks = 400;   // number of msecs before a click is detected.
  unsigned int _pressTicks = 800;   // number of msecs before a long button press is detected

  int _buttonPressed;

  // These variables will hold functions acting as event source.
  callbackFunction _clickFunc = NULL;
  parameterizedCallbackFunction _paramClickFunc = NULL;
  void *_clickFuncParam = NULL;

  callbackFunction _doubleClickFunc = NULL;
  parameterizedCallbackFunction _paramDoubleClickFunc = NULL;
  void *_doubleClickFuncParam = NULL;

  callbackFunction _multiClickFunc = NULL;
  parameterizedCallbackFunction _paramMultiClickFunc = NULL;
  void *_multiClickFuncParam = NULL;

  callbackFunction _longPressStartFunc = NULL;
  parameterizedCallbackFunction _paramLongPressStartFunc = NULL;
  void *_longPressStartFuncParam = NULL;

  callbackFunction _longPressStopFunc = NULL;
  parameterizedCallbackFunction _paramLongPressStopFunc = NULL;
  void *_longPressStopFuncParam;

  callbackFunction _duringLongPressFunc = NULL;
  parameterizedCallbackFunction _paramDuringLongPressFunc = NULL;
  void *_duringLongPressFuncParam = NULL;

  // These variables that hold information across the upcoming tick calls.
  // They are initialized once on program start and are updated every time the
  // tick function is called.

  // define FiniteStateMachine
  enum stateMachine_t : int {
    OCS_INIT = 0,
    OCS_DOWN = 1,
    OCS_UP = 2,
    OCS_COUNT = 3,
    OCS_PRESS = 6,
    OCS_PRESSEND = 7,
    UNKNOWN = 99
  };

  /**
   *  Advance to a new state and save the last one to come back in cas of bouncing detection.
   */
  void _newState(stateMachine_t nextState);

  stateMachine_t _state = OCS_INIT;
  stateMachine_t _lastState = OCS_INIT; // used for debouncing

  unsigned long _startTime; // start of current input change to checking debouncing
  int _nClicks;             // count the number of clicks with this variable
  int _maxClicks = 1;       // max number (1, 2, multi=3) of clicks of interest by registration of event functions.
};

#endif
