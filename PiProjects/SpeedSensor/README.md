# Speed Sensor Data Reader 

## Overview

This directory contains C++ project used for reading speed sensor data from a Raspberry Pi Pico board embedded within motorcycles magnetoresistive sensors.
## Table of Contents

- [Requirements](#requirements)
- [Installation](#installation)

## Requirements

Before running the project, make sure you have the following prerequisites installed and set up:

- [Raspberry Pi Pico](https://www.raspberrypi.org/products/raspberry-pi-pico/) with appropriate firmware found in PicoProjects directory.
- [CMake](https://cmake.org/) (for building the project).
- [GCC](https://gcc.gnu.org/) or [Clang](https://clang.llvm.org/) (C/C++ compiler).
- [Paho MQTT C++ Library](https://github.com/eclipse/paho.mqtt.cpp).

## Installation

1. Download this directory, then:

```bash
cd path/to/SpeedSensor
mkdir build
cd build
cmake ..
make
```

