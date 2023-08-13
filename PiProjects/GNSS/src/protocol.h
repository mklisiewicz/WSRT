#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "i2c.h"


#define RTCM3_PREAMBLE 0xD3
#define NMEA_PREAMBLE 0x24
#define ENDLINE '\n'
#define UTC 1
#define LATITUDE 2
#define LONGITUDE 4
#define MODE 6
#define N_SAT 7
#define HDOP 8
#define ORTHO_HEIGHT 9
#define SPEED 8
#define COURSE 9

double millis();

void write_to_file(std::vector<std::string> msg_vector);

std::string decodeCords(std::string cord);

std::vector<std::string> processMessage(std::string msg);

std::vector<std::string> readNMEA(int i2cHandle);

void sendRTCM(int i2cHandle);