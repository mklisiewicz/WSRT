#include "sensor.hpp"
#include "pico/multicore.h"

#define SDA_PIN 0
#define SCL_PIN 1
#define I2C_ADDR 0x3E
#define I2C_BAUDRATE 100000


uint8_t tx[8];

Sensor frontSensor(FRONT_SENSOR_PIN);
Sensor rearSensor(REAR_SENSOR_PIN);

static void i2cHandler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    switch (event) {
        case I2C_SLAVE_REQUEST:
            std::memcpy(tx, &frontSensor.rpm, 4);
            std::memcpy(tx + 4, &rearSensor.rpm, 4);
            i2c_write_raw_blocking(i2c, tx, 8);
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

void interruptHandler(uint gpio, uint32_t events){
    if (gpio == FRONT_SENSOR_PIN)
        frontSensor.calculateRPM();
    else
        rearSensor.calculateRPM();
}

void setupRefs(){
    gpio_init(REAR_SENSOR_REF);
    gpio_set_dir(REAR_SENSOR_REF, false);
    gpio_pull_up(REAR_SENSOR_REF);

    gpio_init(FRONT_SENSOR_REF);
    gpio_set_dir(FRONT_SENSOR_REF, false);
    gpio_pull_up(FRONT_SENSOR_REF);
}

int main() {
    stdio_init_all();
    setupI2C();
    setupRefs();
    gpio_set_irq_enabled_with_callback(FRONT_SENSOR_PIN, GPIO_IRQ_EDGE_RISE, true, &interruptHandler);
    gpio_set_irq_enabled_with_callback(REAR_SENSOR_PIN, GPIO_IRQ_EDGE_RISE, true, &interruptHandler);
    while (true) {
        frontSensor.setZero();
        rearSensor.setZero();
    }
}