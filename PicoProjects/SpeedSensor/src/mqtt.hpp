#pragma once
#include "mqtt/async_client.h"

#define FRONT 0
#define REAR 1

const std::string ADDRESS = "localhost:1883";
const std::string TOPIC = "bike/sensor/wheel_speed";
const std::string CLIENT_ID = "wheel_speed";
const int QOS = 0;

double get_timestamp();

mqtt::topic setup_mqtt();

void write_mqtt(float wheelSpeed, int side, mqtt::topic& topic);