#pragma once
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include "pico/i2c_slave.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/regs/dreq.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"

#define REAR_SENSOR_PIN 2
#define REAR_SENSOR_REF 4
#define FRONT_SENSOR_PIN 29
#define FRONT_SENSOR_REF 27

#define micros() to_us_since_boot(get_absolute_time())
#define millis() to_ms_since_boot(get_absolute_time())

class Sensor
{
private:
    float deltaTime;
    float start;
    float lastVal;
    uint64_t rstTime;
    int pulse;
    bool check;
public:
    float rpm;

    Sensor(uint gpio);
    void setZero();
    void calculateRPM();   
};