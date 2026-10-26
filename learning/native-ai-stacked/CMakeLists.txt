# Umicom Framework | Native stacked-decoder laboratory | MIT
# Sammy Hegab · Umicom Foundation
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeStackedDecoder LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomNativeStackedDecoder.cmake")
