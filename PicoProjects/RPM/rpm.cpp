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
#include "pico/i2c_slave.h"

#define micros() to_us_since_boot(get_absolute_time())
#define millis() to_ms_since_boot(get_absolute_time())

#define FRONT_SENSOR_PIN 2
#define REAR_SENSOR_PIN 3 
#define SDA_PIN 0
#define SCL_PIN 1
#define I2C_ADDR 0x3E
#define I2C_BAUDRATE 100000

struct SensorData {
    float rpm;
    float deltaTime;
    float start;
    int pulse;
    float lastVal;
    uint64_t rstTime;
    bool check;
};

SensorData frontSensor, rearSensor;

static struct {
    uint8_t tx[8];
    uint8_t test[4] = {1, 2, 3, 4};
    uint8_t tx_pos = 0;
} context;

/////////////////////////// I2C ///////////////////////////

static void i2cHandler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    switch (event) {
        case I2C_SLAVE_REQUEST:
            std::memcpy(context.tx, &frontSensor.rpm, 4);
            std::memcpy(context.tx + 4, &rearSensor.rpm, 4);
            i2c_write_byte_raw(i2c, context.tx[context.tx_pos]);
            context.tx_pos = (++context.tx_pos) % 8;
            break;
        default:
            break;
    }
}

static void setupI2C() {
    gpio_init(SDA_PIN);
    gpio_pull_up(SDA_PIN);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);

    gpio_init(SCL_PIN);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SCL_PIN);

    i2c_init(i2c0, I2C_BAUDRATE);
    i2c_slave_init(i2c0, I2C_ADDR, &i2cHandler);
}

/////////////////////////// SENSOR ///////////////////////////
void calculateRPM(SensorData &sensor) {
    sensor.rpm = (pow(10, 6) * 60) / (5 * sensor.deltaTime);
}

void frontSensorInterrupt(uint gpio, uint32_t events) {
    frontSensor.pulse++;
    if (frontSensor.pulse == 1) {
        frontSensor.deltaTime = micros() - frontSensor.start;
        frontSensor.start = micros();
        frontSensor.pulse = 0;
        calculateRPM(frontSensor);
    }
}

void rearSensorInterrupt(uint gpio, uint32_t events) {
    rearSensor.pulse++;
    if (rearSensor.pulse == 1) {
        rearSensor.deltaTime = micros() - rearSensor.start;
        rearSensor.start = micros();
        rearSensor.pulse = 0;
        calculateRPM(rearSensor);
    }
}

void setupInterrupt() {
    gpio_set_irq_enabled_with_callback(FRONT_SENSOR_PIN, GPIO_IRQ_EDGE_RISE, true, &frontSensorInterrupt);
    gpio_set_irq_enabled_with_callback(REAR_SENSOR_PIN, GPIO_IRQ_EDGE_RISE, true, &rearSensorInterrupt);
}

void setupSensor(uint gpio) {
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
}

/////////////////////////// MAIN ///////////////////////////
int main() {
    stdio_init_all();
    setupI2C();
    setupSensor(FRONT_SENSOR_PIN);
    setupSensor(REAR_SENSOR_PIN);
    setupInterrupt();

    while (true) {
        printf("Front RPM: %f, Rear RPM: %f\n", frontSensor.rpm, rearSensor.rpm);

        // Set RPM to 0 if no pulse is detected for 500ms
        if (millis() - frontSensor.rstTime > 500) {
            if (!frontSensor.check) {
                if (frontSensor.lastVal == frontSensor.rpm) {
                    frontSensor.rpm = 0;
                }
                frontSensor.check = true;
                frontSensor.rstTime = millis();
            } else {
                frontSensor.lastVal = frontSensor.rpm;
                frontSensor.check = false;
                frontSensor.rstTime = millis();
            }
        }

        if (millis() - rearSensor.rstTime > 500) {
            if (!rearSensor.check) {
                if (rearSensor.lastVal == rearSensor.rpm) {
                    rearSensor.rpm = 0;
                }
                rearSensor.check = true;
                rearSensor.rstTime = millis();
            } else {
                rearSensor.lastVal = rearSensor.rpm;
                rearSensor.check = false;
                rearSensor.rstTime = millis();
            }
        }
    }
}
