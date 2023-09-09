#include "sensor.hpp"

void Sensor::calculateRPM(){
    deltaTime = micros() - start;
    start = micros();
    rpm = (pow(10, 6) * 60) / (15 * deltaTime);
}

Sensor::Sensor(uint gpio){
    gpio_init(gpio);
    gpio_set_dir(gpio, GPIO_IN);
}

void Sensor::setZero(){
    if (millis() - rstTime > 500) {
        if (!check) {
            if (lastVal == rpm) {
                rpm = 0;
            }
            check = true;
            rstTime = millis();
        } else {
            lastVal = rpm;
            check = false;
            rstTime = millis();
        }
    }
}

