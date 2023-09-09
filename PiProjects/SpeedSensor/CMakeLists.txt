cmake_minimum_required(VERSION 3.3)
project(rpm C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME} 
        ${SOURCES}
        ${HEADERS}
        )

find_package(PahoMqttCpp REQUIRED)
find_library(wiringPi_LIB wiringPi)

include_directories(${PAHO_MQTT_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}
${wiringPi_LIB}
paho-mqttpp3
paho-mqtt3as
)

