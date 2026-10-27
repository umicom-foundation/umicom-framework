# Native opt-in C23 educational benchmark. No alternative decoder source.
# AUTHOR: Sammy Hegab / Umicom Foundation | LICENCE: MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeStackedBenchmark LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiStackedBenchmark.cmake")
