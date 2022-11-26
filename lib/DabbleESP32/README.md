Whether you’re a student, a teacher, or a hobbyist, Dabble is the perfect place for all your DIYing needs. It transforms your Smartphone into a virtual I/O device and lets you control hardware via Bluetooth, communicate with it, access sensors like accelerometer, GPS, proximity and other features of your Smartphone. It also provides you with dedicated projects compatible with Scratch and Arduino to help you learn by doing. Currently Dabble is avaiable for Android users(coming soon for iPhone users) and its is supported on Android version 5.0.0 and above. One can download the app from Google PlayStore with this link given below. [https://play.google.com/store/apps/details?id=io.dabbleapp]

The app is designed to provide data of certain features of your smartphone to evive and microcontroller boards like Arduino Uno, Nano and Mega and ESP32.It also allows you to control them by your Smartphone. The app communicates with these boards via bluetooth modules like HC-05, HC-06 and HM-10 in case of Arduino boards and with built-in bluetooth for ESP32. This repository consists of library required on your board side for communication with app using BLE on ESP32. Dabble consists of various modules that allows you to control your hardware and access various smartphone features. A brief description of each module is mentioned below:

1) Led Brightness Control: This module allows you to control digital pin of your hardware through mobile. You can make pins HIGH or LOW, can also change its PWM if PWM functionailty is supported on that pin. You can use this module to control LED brightness and carry other such activities.
2) Terminal: Send and receive text and voice commands over Bluetooth between your hardware and smartphone.
3) Gamepad: Control devices in analog (Joystick), digital, and accelerometer mode.
4) Pin State Monitor: Remotely monitor the live status of devices and debug them.
5) Motor Control: Control actuators such as the DC motor and servo motor.
6) Inputs: Provide analog and digital inputs via buttons, knobs, and switches.
7) Camera: Use the camera of your Smartphone for taking photos and videos and colour detection.
8) Phone Sensor: Access different sensors of your Smartphone such as the accelerometer, gyroscope, proximity sensor,magnetometer, light meter, sound meter, GPS, temperature sensor, and barometer to make projects and conduct experiments.
9) Oscilloscope: Visualise and analyse the input and output signals given to the device using the oscilloscope module.
10) IoT: This module consist of Data Logger Module. Module to publish and fetch data will be available soon.
11) Music: Receive commands from the device and play tones, songs, or other recorded files on your Smartphone.
12) Projects: Make dedicated projects to experience different concepts of the real world first-hand.
Dabble Library
As per the module set present in Dabble app the library is also structured in same way. This library is for ESP32. It consists of certain examples codes which will help you in getting started with various Dabble modules. To explore more about Dabble, goto [https://thestempedia.com/docs/dabble/getting-started-with-dabble/]
