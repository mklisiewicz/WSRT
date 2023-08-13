#include <iostream>
#include <vector>
#include <string>
#include "i2c.h"
#include "protocol.h"
#include "config.h"
#include <unistd.h>

int setup() {
    uint64_t start = millis();
    int i2cHandle = i2cOpen();
    i2cSetAddress(i2cHandle);
    sendConfig(i2cHandle);
    std::cout << "Waiting for connection..." << std::endl;
    while (millis() - start < 1000){}
    return i2cHandle;
}

int main() {
    int i2cHandle = setup();
    std::string line = "$GNGNS,055504.85,5207.06610,N,02051.21600,E,AAAANN,25,0.70,114.5,34.6,,,V*1C";
    while (true) {
        sendRTCM(i2cHandle);
        readNMEA(i2cHandle);
    }
    close(i2cHandle);
    return 0;
}