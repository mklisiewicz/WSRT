#include "i2c.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <chrono>

#define RCV_ADDR 0x42

int i2cOpen() {
    int i2cHandle; 
    while ((i2cHandle = open("/dev/i2c-1", O_RDWR))<0){
        std::cerr << "Failed to open I2C device." << std::endl;
    }
    return i2cHandle;
}

void i2cSetAddress(int i2cHandle) {
    while (ioctl(i2cHandle, I2C_SLAVE, RCV_ADDR) < 0) {
        std::cerr << "Failed to set I2C address." << std::endl;
    }
}

void i2cWrite(const std::vector<uint8_t> data, int i2cHandle) {
    if (write(i2cHandle, data.data(), data.size()) != data.size()) {
        std::cerr << "Failed to send I2C data." << std::endl;
    }
}