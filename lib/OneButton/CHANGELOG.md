# Changelog

All notable changes to this project will be documented in this file starting 2021.

## [2.0.4] - 2022-01-22

* checked for ESP32 (SimpleOneButton, InterruptOneButton, BlinkMachine)
and included example PIN definitions for ESP32
* Documentation changes

## [2.0.3] - 2021-10-26

* fixing parameter missuse and potential crash

## [2.0.1] - 2021-01-31

* Compiler warning removed
* Documentation

## [2.0.0] - 2021-01-22 

* CHANGELOG created.
* Many thanks to the improvements included from #27 (**@aslobodyanuk**), #59 (**@ShaggyDog18**) and #73 (**@geeksville**).

This is a major update with breaking changes.

The **states** are re-factored to support counting the clicks.

By design only one of the events (click, doubleClick, MultiClick) are triggered within one interaction.
As a consequence a single-click interaction is detected after waiting some milliseconds (see setClickTicks()) without another click happening;
Only if you have not attached any double-click event function the waiting time can be skipped.

Detecting a long 'down' not only works with the first but always as the last click.

The number of actual clicks can be retrieved from the library any time.

The function **getPressedTicks()** was removed. See example SimpleOneButton on how to get that time by using attachLongPressStart to save starting time.

The function **attachPressStart()** is removed as **attachLongPressStart()** does the same but also supports parameters.

One additional feature has been added not to call the event functions from the interrupt routine and detect
the need for event functions to be called only when the tick() function is called from the main loop() method.
This is because some boards and processors do not support timing or Serial functions (among others) from interrupt routines.

The function **isIdle()** was added to allow detect a current interaction.

The library now supports to detect multiple (>2) clicks in a row using **attachMultiClick()** .


* The internal _state is using enum instead of plain numbers to make the library more readable.
* functions that had been marked deprecated are now removed. (attachPress->attachLongPressXXX)
* added const to constant parameters to enable meaningful compiler warnings.
* added code for de-bouncing double clicks from pull 27.
* added isIdle() function to find out that the internal state is `init`. 


### Examples

* Examples run on NodeMCU boards. (the library worked already).

* The **SimpleOneButton.ino** example got some cleanup and definition to be used with ESP8266 boards as well.

* The **InterruptOneButton.ino** example now is using attachInterrupt instead of UNO specific register modifications.

* The **SpecialInput.ino** example was added to shpow how to use the OneButton algorythm and input pattern recognition with your own source of input.
