#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <fcntl.h>
#include <stdio.h>
#include <chrono>
#include <algorithm>

#define RCV_ADDR 0x42
#define RTCM3_PREAMBLE 0xD3
#define NMEA_PREAMBLE 0x24
#define ENDLINE 0x0A
#define UTC 1
#define LATITUDE 2
#define LONGITUDE 4
#define MODE 6
#define N_SAT 7
#define HDOP 8

/*void write_to_file(uint8_t *ptr, size_t len) {
    std::ofstream output_file("/home/mateusz/gps/nmea_messages.csv", std::ios::app);
    output_file.write((char*)ptr, len);
    output_file.close();
}*/


std::vector<uint8_t> decodeCords(std::vector<uint8_t> cord) {
    double minutes;
    double degrees;
    std::string cord_str(cord.begin(), cord.end());
    std::vector<uint8_t> decimal_degrees;
    std::string decimal_degrees_str;
    for (int i = 0; i < cord_str.length(); i++) {
        if (cord[i] == '.') {
            minutes = std::stod(cord_str.substr(i-2, i+5));
            degrees = std::stod(cord_str.substr(0, i-2));
            break;
        }
    }
    decimal_degrees_str = std::to_string(degrees + minutes/60);
    for (int i = 0; i < decimal_degrees_str.length(); i++) {
        decimal_degrees.push_back(decimal_degrees_str[i]);
    }
    return decimal_degrees;
}

void processGNS(std::vector<uint8_t> gns_msg, int size) {
    int field = 0;
    std::vector<std::vector<uint8_t>> prcsd_msg_vctr;
    std::vector<uint8_t> buffer;
    for (int i = 0; i < size; i++) {
        if (gns_msg[i] == ',') {
            field++;
            prcsd_msg_vctr.push_back(buffer);
            buffer.clear();
        }
        else{
            buffer.push_back(gns_msg[i]);
        }
    }
    prcsd_msg_vctr.push_back(buffer);
    buffer.clear();
    
    prcsd_msg_vctr[LONGITUDE] = decodeCords(prcsd_msg_vctr[LONGITUDE]);
    prcsd_msg_vctr[LATITUDE] = decodeCords(prcsd_msg_vctr[LATITUDE]);
    
    prcsd_msg_vctr[UTC].insert(prcsd_msg_vctr[UTC].begin()+2, ':');
    prcsd_msg_vctr[UTC].insert(prcsd_msg_vctr[UTC].begin()+5, ':');

    for (size_t i = 0; i < prcsd_msg_vctr.size(); i++)
    {
        if (i != 0) {
            prcsd_msg_vctr[i].insert(prcsd_msg_vctr[i].begin(), ',');
        }
        for (size_t j = 0; j < prcsd_msg_vctr[i].size(); j++)
        {
            std::cout << prcsd_msg_vctr[i][j];
        }
        
    }
    

    /*for (int i = 0; i < prcsd_msg_vctr.size(); i++) {
        if (i != 0) {
            prcsd_msg[k] = ',';
            k++;        
        }
        for (int j = 0; j < prcsd_msg_vctr[i].length(); j++) {
            
            prcsd_msg[k] = prcsd_msg_vctr[i][j];
            k++;
        }
    }*/

    //write_to_file(prcsd_msg, k);
}

/*void readNMEA(int i2cHandle) {
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
        if (received_bytes[3] == 'G') {processGNS(received_bytes, i+1);}
        else {processGNS(received_bytes, i+1);}
    }
}*/

int main() {
    std::string line = "$GNGNS,055504.85,5207.06610,N,02051.21600,E,AAAANN,25,0.70,114.5,34.6,,,V*1C";
    std::vector<uint8_t> gns_msg;
    for (int i = 0; i < line.length(); i++) {
        gns_msg.push_back(line[i]);
    }
        //readNMEA(i2cHandle);
        //readRTCM(i2cHandle);
        processGNS(gns_msg, gns_msg.size());
    return 0;
}
