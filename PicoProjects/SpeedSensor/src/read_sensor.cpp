#include "read_sensor.hpp"

uint16_t FRONT_WHEEL_DIAMETER;
uint16_t REAR_WHEEL_DIAMETER;

void getConfig(){
    std::vector<uint16_t> diameters;
    std::ifstream config (CONFIG_FILE);
    if (config.is_open())
    {
        std::string line;
        while(getline(config, line))
       {
            line.erase(std::remove_if(line.begin(), line.end(), isspace),
                                 line.end());
            if( line.empty() || line[0] == '#' )
            {
                continue;
            }
            auto delimiterPos = line.find("=");
            auto value = line.substr(delimiterPos+1);
            diameters.push_back(stoi(value));
        }
        FRONT_WHEEL_DIAMETER = diameters[0];
        REAR_WHEEL_DIAMETER = diameters[1];
    }
    else 
    {
        std::cerr << "Couldn't open config file for reading.\n";
    }
}

float calculateSpeed(float rpm, int side){
    float wheelDiameter;
    switch (side)
    {
    case FRONT:
        wheelDiameter = FRONT_WHEEL_DIAMETER;
        break;
    case REAR:
        wheelDiameter = REAR_WHEEL_DIAMETER;
    default:
        break;
    }

    float speed = M_PI * (wheelDiameter/1000000) * 60 *rpm;
    return speed;
}

std::pair<float, float> readData() {
    const char *i2cDevice = "/dev/i2c-1"; // I2C device file for Raspberry Pi (may vary)
    int i2config = open(i2cDevice, O_RDWR);

    if (i2config < 0) {
        std::cerr << "Error opening I2C bus" << std::endl;
        return {-1, -1};
    }

    if (ioctl(i2config, I2C_SLAVE, I2C_ADDR) < 0) {
        std::cerr << "Error setting I2C address" << std::endl;
        close(i2config);
        return {-1, -1};
    }

    uint8_t data[8];

    if (read(i2config, data, sizeof(data)) != sizeof(data)) {
        std::cerr << "Error reading data from I2C device" << std::endl;
        close(i2config);
        return {-1, -1};
    }

    close(i2config);

    float frontRpm, rearRpm;
    std::memcpy(&frontRpm, data, sizeof(float));
    std::memcpy(&rearRpm, data + sizeof(float), sizeof(float));

    float frontSpeed = calculateSpeed(frontRpm, FRONT);
    float rearSpeed = calculateSpeed(rearRpm, REAR);
    std::pair<float, float> values = {frontSpeed, rearSpeed};
    return values;
}


