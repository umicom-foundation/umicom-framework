# Umicom Framework: Native K/V capture from the real stacked Transformer.
# Sammy Hegab / Umicom Foundation | MIT
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeStackedKvCapture LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiStackedKvCapture.cmake")
