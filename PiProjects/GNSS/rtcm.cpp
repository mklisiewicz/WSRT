#include <iostream>
#include <vector>
#include <fstream>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define RCV_ADDR 0x42
#define CHUNK_SIZE 128

int i2cOpen() {
    int i2cHandle = open("/dev/i2c-1", O_RDWR);
    if (i2cHandle < 0) {
        std::cerr << "Failed to open I2C device." << std::endl;
        return -1;
    }
    return i2cHandle;
}

void i2cSetAddress(int i2cHandle) {
    if (ioctl(i2cHandle, I2C_SLAVE, RCV_ADDR) < 0) {
        std::cerr << "Failed to set I2C address." << std::endl;
    }
}

void i2cWrite(const std::vector<uint8_t>& data, int i2cHandle) {
    if (write(i2cHandle, data.data(), data.size()) != data.size()) {
        std::cerr << "Failed to send I2C data." << std::endl;
    }
}

void sendRTCM(const std::vector<uint8_t>& data, int i2cHandle) {
    i2cSetAddress(i2cHandle);
    i2cWrite(data, i2cHandle);
}

void readRTCM(int i2cHandle) {
    std::vector<uint8_t> data_chunk(CHUNK_SIZE);
    std::cin.read(reinterpret_cast<char*>(data_chunk.data()), CHUNK_SIZE);
    size_t data_read = std::cin.gcount();
    if (data_read > 0) {
        std::vector<uint8_t> data(data_chunk.begin(), data_chunk.begin() + data_read);
        sendRTCM(data, i2cHandle);
        }
}

void readNMEA(int i2cHandle) {
    std::string received_data;
    uint8_t received_byte;
    bool save = false;
    if (read(i2cHandle, &received_byte, 1) != 1) {
        std::cerr << "Failed to read from I2C." << std::endl;
    }
    else if (received_byte == '$'){
        save = true;
    }
    if (save){
        received_data.push_back(received_byte);
        if (received_byte == '\n') {
            // Process and save NMEA message to a file
            std::ofstream output_file("/home/mateusz/gps/nmea_messages.txt", std::ios::app);
            if (output_file.is_open()) {
                output_file << received_data;
                output_file.close();
            } else {
                std::cerr << "Failed to open output file." << std::endl;
            }
            received_data.clear();
            save = false;
        }
    }
}

int main() {
    int i2cHandle = i2cOpen();
    i2cSetAddress(i2cHandle);
    while (true) {
        //readRTCM(i2cHandle);
        readNMEA(i2cHandle);
    }
    close(i2cHandle);
    return 0;
}
