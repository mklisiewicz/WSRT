#include <iostream>
#include <vector>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define RCV_ADDR 0x42
#define CHUNK_SIZE 128

bool sendRTCM(const std::vector<uint8_t>& data) {
    int i2cHandle = open("/dev/i2c-1", O_RDWR);
    if (i2cHandle < 0) {
        std::cerr << "Failed to open I2C device." << std::endl;
        return false;
    }

    if (ioctl(i2cHandle, I2C_SLAVE, RCV_ADDR) < 0) {
        std::cerr << "Failed to set I2C address." << std::endl;
        close(i2cHandle);
        return false;
    }

    if (write(i2cHandle, data.data(), data.size()) != data.size()) {
        std::cerr << "Failed to send I2C data." << std::endl;
        close(i2cHandle);
        return false;
    }
    
    close(i2cHandle);
    return true;
}

int main() {
    std::vector<uint8_t> data_chunk(CHUNK_SIZE);

    while (true) {
        std::cin.read(reinterpret_cast<char*>(data_chunk.data()), CHUNK_SIZE);
        size_t data_read = std::cin.gcount();

        if (data_read > 0) {
            std::vector<uint8_t> data(data_chunk.begin(), data_chunk.begin() + data_read);
            if (!sendRTCM(data)) {
                std::cerr << "Failed to send data over I2C." << std::endl;
            }
        }
    }

    return 0;
}