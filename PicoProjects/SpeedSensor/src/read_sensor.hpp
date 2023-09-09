#pragma once
#include <iostream>
#include <cstdint>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <wiringPi.h>
#include <cstring>
#include <chrono>
#include <cmath>
#include <vector>
#include <fstream>
#include <algorithm>

#define I2C_ADDR 0x3E
#define FRONT 0
#define REAR 1
#define CONFIG_FILE "/home/mateusz/skrypty/rpm/cfg/wheel_diameters.cfg"

void getConfig();
float calculateSpeed(float rpm, int side);
std::pair<float, float> readData();
