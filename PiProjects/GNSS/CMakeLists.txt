cmake_minimum_required(VERSION 3.15)
project(gnss)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCES ${SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE HEADERS ${SOURCE_DIR}/*.h)

add_executable(gnss 
        ${SOURCES}
        ${HEADERS}
        )

target_include_directories(gnss PRIVATE ${SOURCE_DIR})