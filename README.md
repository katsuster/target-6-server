# target-6-server

`target-6` is an opensource steel targets shooting game.

This is a server program to run real-time jobs for example, detect hit signals from target sensors and calculate the time in sub-milliseconds.
The server and client communicate with Bluetooth Low Energy (BLE) protocol.

This server program runs on small hardware such as IoT hardware.
The hardware is attached on the other board and receives various singnals through it.

Please refer these repositories if you have interest in other modules of `target-6`.

* Client: https://github.com/katsuster/target-6-client
* Board: https://github.com/katsuster/target-6-board


## Requires

* Software
  * Arduino IDE
  * Boards Manager: M5Stack
* Hardware
  * M5Stamp C3


## How to use

* [Setup Arduino IDE](doc/setup_arduino.md)
* [Build and Upload](doc/build.md)


## Source structures

* c3/
  * For M5Stamp C3


## Other documents

* [GPIO](doc/gpio.md)
