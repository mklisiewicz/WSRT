#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include "protocol.h"
#include "i2c.h"

unsigned long timestamp = millis();

double millis() {
    return std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::system_clock::now().time_since_epoch()
            ).count();
}

void write_to_file(std::vector<std::string> msg_vector) {
    std::ofstream output_file("/home/catreson/dane_test/nmea_messages" + std::to_string(timestamp) + ".csv", std::ios::app);
    output_file<<std::to_string(millis());
    output_file<<',';
    for (int i = 0; i < msg_vector.size(); i++) {
        output_file << millis();
        output_file << ",";
        output_file << msg_vector[i];
    }
    output_file.close();
}

std::string decodeCords(std::string cord) {
    double minutes, degrees;
    std::string decimal_degrees;
    
    for (int i = 0; i < cord.length(); i++) {
        if (cord[i] == '.') {
            minutes = std::stod(cord.substr(i-2, i+5));
            degrees = std::stod(cord.substr(0, i-2));
            break;
        }
    }

    decimal_degrees = std::to_string(degrees + minutes/60);

    return decimal_degrees;
}

std::vector<std::string> processMessage(std::string msg) {
    std::vector<std::string> msg_vector, prcsd_msg_vector;
    std::string buffer;
    
    for (int i = 0; i < msg.size(); i++) {
        if (msg[i] == ',') {
            msg_vector.push_back(buffer);
            buffer.clear();
        }
        else{
            buffer.push_back(msg[i]);
        }
    }
    msg_vector.push_back(buffer);
    buffer.clear();
    
    if (msg[3] == 'G') {
        msg_vector[LONGITUDE] = decodeCords(msg_vector[LONGITUDE]);
        msg_vector[LATITUDE] = decodeCords(msg_vector[LATITUDE]);
        
        prcsd_msg_vector.push_back(msg_vector[UTC]);
        prcsd_msg_vector.push_back(msg_vector[LONGITUDE]);
        prcsd_msg_vector.push_back(msg_vector[LATITUDE]);
        prcsd_msg_vector.push_back(msg_vector[N_SAT]);
        prcsd_msg_vector.push_back(msg_vector[HDOP]);
        prcsd_msg_vector.push_back(msg_vector[MODE]);
        prcsd_msg_vector.push_back(msg_vector[ORTHO_HEIGHT]);
        prcsd_msg_vector.push_back(",");
    }
    else {
        prcsd_msg_vector.push_back(msg_vector[SPEED]);
        prcsd_msg_vector.push_back(msg_vector[COURSE]);
        prcsd_msg_vector.push_back(msg_vector[ENDLINE]);

    }
    return prcsd_msg_vector;
}

std::vector<std::string> readNMEA(int i2cHandle) {
    std::string received_bytes;
    uint8_t received_byte;

    if (read(i2cHandle, &received_byte, 1) == 1 && received_byte == NMEA_PREAMBLE) {
        received_bytes.push_back(received_byte);

        while(read(i2cHandle, &received_byte, 1) == 1 && received_byte != ENDLINE && received_byte > 0x20) {
            received_byte &= ~(1 << 7);
            received_bytes.push_back(received_byte);
        }
    }
    write_to_file(processMessage(received_bytes));
}

void sendRTCM(int i2cHandle) {
    std::vector<uint8_t> rtcm_bytes;
    char rtcm_byte;
    if (std::cin.get(rtcm_byte) && rtcm_byte == RTCM3_PREAMBLE) {
        std::string line;
        std::getline(std::cin, line, ENDLINE);  
        std::cout << line.length() << std::endl;
        for (char c : line) {
            rtcm_bytes.push_back(reinterpret_cast<uint8_t&>(c));
        }
        i2cWrite(rtcm_bytes, i2cHandle);
    }
}

