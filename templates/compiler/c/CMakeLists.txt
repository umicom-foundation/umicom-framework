# Umicom C23 application template | Sammy Hegab | Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomC23Template LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_executable(umicom-c23 src/main.c)
