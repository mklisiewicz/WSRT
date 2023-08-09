#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <chrono>
#include <algorithm>


#define RCV_ADDR 0x42
#define RTCM3_PREAMBLE 0xD3
#define NMEA_PREAMBLE 0x24
#define ENDLINE 0x0A

const uint8_t USB_SET[]{
    0xB5, 0x62, 0x06, 0x8A, 0x18, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x01, 0x00,
    0x78, 0x10, 0x01, 0x01, 0x00, 0x78,
    0x10, 0x01, 0x02, 0x00, 0x78, 0x10,
    0x01, 0x01, 0x00, 0x77, 0x10, 0x01, 
    0xD1, 0x28, 0x00, 0x00
};

const uint8_t DISABLE_UART1[] = {
    0xB5, 0x62, 0x06, 0x00, 0x14, 0x00,
    0x01, 0x00, 0x00, 0x00, 0xD0, 0x08,
    0x00, 0x00, 0x00, 0x96, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x89, 0x46};

const uint8_t DISABLE_UART2[] = {
    0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 
    0x02, 0x00, 0x00, 0x00, 0xD0, 0x08, 
    0x00, 0x00, 0x00, 0x96, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x8A, 0x5A};


const uint8_t DISABLE_USB[] = {
    0xB5, 0x62, 0x06, 0x00, 0x14, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x1D, 0x84};

const uint8_t DISABLE_SPI[] = {
    0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 
    0x04, 0x00, 0x00, 0x00, 0x00, 0x32, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x50, 0x86};

const uint8_t SET_RATE_UTC[] = {
    0xB5, 0x62, 0x06, 0x08, 0x06, 0x00,
    0x19, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x2E, 0x4E
};

const uint8_t SET_RATE_GPS[] = {
    0xB5, 0x62, 0x06, 0x08, 0x06, 0x00,
    0x19, 0x00, 0x01, 0x00, 0x01, 0x00,
    0x2F, 0x50
};

const uint8_t SET_RATE_GAL[] = {
    0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 
    0x19, 0x00, 0x01, 0x00, 0x02, 0x00, 
    0x30, 0x52
};

const uint8_t SET_RATE_GLO[] = {
    0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 
    0x19, 0x00, 0x01, 0x00, 0x03, 0x00, 
    0x31, 0x54
};

const uint8_t SET_RATE_BDS[] = {
    0xB5, 0x62, 0x06, 0x08, 0x06, 0x00, 
    0x19, 0x00, 0x01, 0x00, 0x04, 0x00, 
    0x32, 0x56,
};

const uint8_t SET_I2C_OUT[] = {

};

const uint8_t SET_I2C_IN[] = {

};

const uint8_t* commands[] = {
    DISABLE_UART1,
    DISABLE_UART2,
    //DISABLE_USB,
    DISABLE_SPI,
    SET_RATE_UTC,
    SET_RATE_GPS,
    SET_RATE_GAL,
    SET_RATE_GLO,
    SET_RATE_BDS,
    USB_SET
};


void sendConfig(int i2cHandle) {
    for (int i = 0; i < sizeof(commands); i++){
        write(i2cHandle, commands[i], sizeof(commands));
    }
}

unsigned long millis() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::high_resolution_clock::now().time_since_epoch()
           ).count();
}

void write_to_file(uint8_t *ptr, size_t len) {
    std::ofstream output_file("/home/mateusz/gps/nmea_messages.csv", std::ios::app);
    output_file.write((char*)ptr, len);
    output_file.close();
}

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

void readRTCM(int i2cHandle) {
    std::vector<uint8_t> rtcm_bytes;
    char rtcm_byte;
    if (std::cin.get(rtcm_byte) && rtcm_byte == RTCM3_PREAMBLE) {
        std::string line;
        std::getline(std::cin, line, '\n');  
        std::cout << line.length() << std::endl;
        for (char c : line) {
            rtcm_bytes.push_back(reinterpret_cast<uint8_t&>(c));
        }
        i2cWrite(rtcm_bytes, i2cHandle);
    }
}

void readNMEA(int i2cHandle) {
    uint8_t received_bytes[92] = {'$'};
    uint8_t received_byte;
    int i = 1;

    if (read(i2cHandle, &received_byte, 1) == 1 && received_byte == NMEA_PREAMBLE) {
        while(read(i2cHandle, &received_byte, 1) == 1 && received_byte != ENDLINE && received_byte > 0x20) {
            received_byte &= ~(1 << 7);
            received_bytes[i] = received_byte;
            i++;
        }
        received_bytes[i] = ENDLINE;
        write_to_file(received_bytes, i+1);
    }
}

int setup() {
    unsigned long startTime = millis();
    int i2cHandle = i2cOpen();
    i2cSetAddress(i2cHandle);
    sendConfig(i2cHandle);
    while (millis() - startTime < 1000){
        std::cout << "Waiting for GPS to start..." << std::endl;
    }
    return i2cHandle;
}

int main() {
    int i2cHandle = setup();

    while (true) {
        readNMEA(i2cHandle);
        readRTCM(i2cHandle);
    }
    close(i2cHandle);
    return 0;
}
