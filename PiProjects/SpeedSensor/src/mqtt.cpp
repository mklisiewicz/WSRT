#include "mqtt.hpp"

mqtt::topic setup_mqtt(){
    mqtt::async_client client(ADDRESS, CLIENT_ID);
    mqtt::topic topic(client, TOPIC, QOS, true);

    try{
        client.connect()->wait();
    }
    catch(std::exception){
        std::cerr<<"No client";
    }
    return topic;
}

double get_timestamp(){
    double fractional_seconds_since_epoch
    = std::chrono::duration_cast<std::chrono::duration<double>>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    return fractional_seconds_since_epoch;
}

void write_mqtt(float wheelSpeed, int side, mqtt::topic& topic) {
    std::string tag; 
    switch (side)
    {
    case FRONT:
        tag = "wheel_f";
        break;
    case REAR:
        tag = "wheel_r";
    default:
        break;
    }

    std::string message = tag + "," + std::to_string(get_timestamp()) + "," + std::to_string(wheelSpeed) + "," + TOPIC + "," + "float"; 
    try{
		topic.publish(message);
    }

    catch(std::exception exc){
        std::cout<<"No mqtt\n";
    }

}
