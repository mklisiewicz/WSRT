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

#define SENSOR_PIN 4
#define SDA_PIN 0
#define SCL_PIN 1
#define I2C_ADDR 0x3E
#define I2C_BAUDRATE 100000

float deltaTime;
float start;
float rpm = 0;
int pulse = 0;

float lastVal = 0;
uint64_t rstTime = millis();
bool check = false;

static struct{
uint8_t tx[4];
uint8_t tx_pos = 0;
} context;
/////////////////////////// I2C ///////////////////////////

static void i2cHandler(i2c_inst_t *i2c, i2c_slave_event_t event) {
    switch (event){
    case I2C_SLAVE_REQUEST:
        std::memcpy(context.tx, &rpm, 4);
        i2c_write_byte_raw(i2c0, context.tx[context.tx_pos]);
        context.tx_pos = (++context.tx_pos)%4;
        break;
    default:
        break;
    }
}

static void setupI2C(){
    gpio_init(SDA_PIN);
    gpio_pull_up(SDA_PIN);
    gpio_set_function(SDA_PIN ,GPIO_FUNC_I2C);

    gpio_init(SCL_PIN);
    gpio_set_function(SCL_PIN ,GPIO_FUNC_I2C);
    gpio_pull_up(SCL_PIN);
    
    i2c_init(i2c0, I2C_BAUDRATE);
    i2c_slave_init(i2c0, I2C_ADDR, &i2cHandler);
}

/////////////////////////// SENSOR ///////////////////////////
void calculateRPM(uint32_t time){
    rpm = (pow(10, 7)*6)/(5*time);
}

void sensorInterrupt(uint gpio, uint32_t events){
    pulse++;
    if(pulse == 1){
        deltaTime = micros() - start;
        start = micros();
        pulse = 0;
        calculateRPM(deltaTime);
    }   
}

void setupInterrupt(){
    gpio_set_irq_enabled_with_callback(SENSOR_PIN, GPIO_IRQ_EDGE_RISE, true, &sensorInterrupt);
}

void setupSensor(){
    gpio_init(SENSOR_PIN);
    gpio_set_dir(SENSOR_PIN, GPIO_IN);
}

/////////////////////////// MAIN ///////////////////////////
int main(){
    stdio_init_all();
    setupI2C();
    setupSensor();
    setupInterrupt();

    while(true){
        //Set RPM to 0 if no pulse is detected for 500ms
        if(millis() - rstTime > 500){
            if (!check){
                if(lastVal == rpm){
                    rpm = 0;
                }
                check = true;
                rstTime = millis();
            }
            else{
                lastVal = rpm;
                check = false;
                rstTime = millis();
            }
        }

    }
}
