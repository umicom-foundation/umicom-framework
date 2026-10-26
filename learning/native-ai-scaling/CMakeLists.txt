# Umicom Framework | Native AI capacity planning | Sammy Hegab | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeAiScaling LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiScaling.cmake")
