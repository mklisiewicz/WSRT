#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/regs/dreq.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include <cstdlib>
#include <cstring>
#include <cmath>

#define micros() to_us_since_boot(get_absolute_time())
#define SENSOR_PIN 4
#define I2C_ADDR 0x3E
#define SDA_PIN 0
#define SCL_PIN 1

float deltaTime;
float start;
float rpm = 0;
int pulse = 0;

void calculateRPM(uint32_t time){
    rpm = (pow(10, 6)*60)/(5*time);
}

void sensorInterrupt(uint gpio, uint32_t events){
    pulse++;
    if(pulse == 3){
        deltaTime = micros() - start;
        start = micros();
        pulse = 0;
        calculateRPM(deltaTime);
        printf("RPM: %f\n", rpm);
    }   
}

void setupInterrupt(){
    gpio_set_irq_enabled_with_callback(SENSOR_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &sensorInterrupt);
}

void setupI2C(){
    i2c_init(i2c0, 100000);
    i2c_set_slave_mode(i2c0, true, I2C_ADDR);
    gpio_set_function(SDA_PIN ,GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN ,GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);
}

void setupSensor(){
    gpio_init(SENSOR_PIN);
    gpio_set_dir(SENSOR_PIN, GPIO_IN);
}

int main(){
    uint8_t txdata[4];
    stdio_init_all();
    setupI2C();
    setupSensor();
    setupInterrupt();

    while(true){
        std::memcpy(txdata, &rpm, 4);
        i2c_write_raw_blocking(i2c0, txdata, 4);
    }
}