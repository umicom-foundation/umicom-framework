# Umicom Framework | Native stacked KV cache storage learning laboratory
# Sammy Hegab / Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeStackedKvStore LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiStackedKvStore.cmake")
