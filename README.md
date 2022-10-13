# BitBus client for Arduino

This library is designed to communicate with the [BlueBit BitBus App](http://play.google.com/store/apps/details?id-com.bitbus.BitBlue).

Author: Eric Z. Ayers<ericzundel@gmail.com>

Repo: [ericzundel/BitBus](https://github.com/ericzundel/BitBus)

## Hardware Setup
Setup your Arduino with a serial Bluetooth module via SoftSerial on pins D2 and D3. Then, you can install the BitBlue BitBus app on your phone.  When you connect to the Bluetooth Module, select "Controller" and then "Mode" to put the app into Analog Joystick Game Controller mode.

## Software Setup
See the example in GamePadDemo for how to use this module.

## Features
- Small footprint.
- Emulates the [STEMpedia Dabble library](https://thestempedia.com/product/dabble/) for easy switching back and forth

# Caveats
- I have only tested this on an Arduino Nano running the 2.0.0 Arduino IDE.
- Only supports the AVR architecture at the moment. I have not tested on other architectures and they will likely break as I used AVR specific code to store a large datastructure and strings in flash.

## Author's Note
I undertook this project to work around a bug in the UI of the Dabble library. In retrospect, I spent way to long making this library nice and pretty, because the app it supports has some problems and we probably shouldn't be using it.

- I can't find a website or webpage describing this app. Not even in Korean.
- The Android app is very old and requires you to open up a ton of permissions. That is a bit scary when you can't read the documentation.
- To my knowledge, the app is not open source and we can't fork or fix these problems.
- The protocol used by the app is not very robust.  For example, it ships out of the box with the down button and the B button sharing the same value. Implementing autodetection of joystick mode vs. digital button mode is impossible without remapping these keys.
- The protocol doesn't have a way to tell you if a button is held down.  I emulate that with the analog joystick but don't have a way to do that with the action buttons. Implementing autodetection of joystick mode vs. digital button mode is impossible without remapping these keys.

Do you want to see a nice protocol for a Game Controller app?  Check out the Dabble implementation.
