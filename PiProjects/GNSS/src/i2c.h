#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <chrono>


int i2cOpen();

void i2cSetAddress(int i2cHandle);

void i2cWrite(const std::vector<uint8_t> data, int i2cHandle);
