#-----------------------------------------------------------------------------
# Umicom Framework / Native LLM Learning Laboratory
# File: learning/native-llm/CMakeLists.txt
#
# PURPOSE:
#   Build and verify the Framework's new training service independently of
#   the much larger Umicom Applications tree. The SAME Framework-owned C23
#   files and CMake composition module are used in a full parent build.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeLlmLaboratory LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomNativeLlmTraining.cmake")

# This tool is a thin native consumer, not a second implementation of training.
add_executable(umicom-native-llm-train
    "${CMAKE_CURRENT_LIST_DIR}/../../learning/native-llm/train_bigram.c")
target_link_libraries(umicom-native-llm-train PRIVATE Umicom::AiTraining)
if(NOT MSVC)
    target_compile_options(umicom-native-llm-train PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
