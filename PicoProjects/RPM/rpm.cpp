#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "pico.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "hardware/regs/dreq.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include <cstdlib>
#include <cmath>

#define millis() to_ms_since_boot(get_absolute_time())

#define I2C_ADDR 0x3E

int main(){
    stdio_init_all();
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    __uint32_t start = millis();
    __uint32_t currentTime;
    __uint32_t deltaTime;

    __uint16_t currentState;
    __uint16_t lastState;
    __uint16_t stateChange;

    int pulse = 0;
    float wheelDiameter = 1.25;
    float rpm;
    float conversionFactor = M_PI*60*wheelDiameter;

    uint8_t txdata[2];

    i2c_init(i2c0, 100000);
    i2c_set_slave_mode(i2c0, true, I2C_ADDR);
    gpio_set_function(0 ,GPIO_FUNC_I2C);
    gpio_set_function(1 ,GPIO_FUNC_I2C);
    gpio_pull_up(0);
    gpio_pull_up(1);

    while(true){
        currentState = adc_read();
        currentTime = millis();
        stateChange = abs(currentState - lastState);
        deltaTime = currentTime - start;
        if (stateChange > 30) {
            pulse++;
        }
        lastState = currentState;
        if(deltaTime >= 500){
            rpm = (pulse*120)/15;
            pulse = 0;
            start = currentTime;
            int speed = rpm * conversionFactor;
            txdata[0] = speed & 0xFF;
            txdata[1] = speed >> 8;
            i2c_write_raw_blocking(i2c0, txdata, 2);
        }
    }
}