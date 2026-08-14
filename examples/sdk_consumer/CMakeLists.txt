# Minimal installed-SDK consumer used by integrators and release validation.
cmake_minimum_required(VERSION 3.24)
project(UmicomSdkConsumer LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS sdk delivery)
add_executable(umicom-sdk-consumer main.c)
target_link_libraries(umicom-sdk-consumer PRIVATE Umicom::sdk Umicom::delivery)
