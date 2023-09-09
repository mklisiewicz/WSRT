#include "read_sensor.hpp"
#include "mqtt.hpp"

unsigned long startMillis = 0;

unsigned long getMillis() {
    auto currentTime = std::chrono::system_clock::now();
    auto duration = currentTime.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

int main() {
    if (wiringPiSetup() == -1) {
        std::cerr << "Error initializing WiringPi" << std::endl;
        return 1;
    }
    getConfig();
    mqtt::topic wheelTopic = setup_mqtt();
    unsigned long start = getMillis();

    while (true) {
        if (getMillis() - start > 20){
            std::pair<float, float> values = readData();
            std::cout << values.first << std::endl;
            std::cout << values.second << std::endl;
            write_mqtt(values.first, FRONT, wheelTopic);
            write_mqtt(values.second, REAR, wheelTopic);
            start = getMillis();
        }
    }

    return 0;
}
