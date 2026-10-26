#-----------------------------------------------------------------------------
# Umicom Framework: reviewed native supervised fine-tuning laboratory
# PURPOSE:
#   Compose Framework-owned source admission, fixed byte training windows,
#   low-rank decoder adaptation, held-out evaluation and evidence receipts.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeSupervisedExperiment LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
include(CTest)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomAiSupervisedExperiment.cmake")

# A thin educational consumer of the SAME Framework services and synthetic
# fixtures used by the regression tests. No second trainer or model maths.
add_executable(umicom-native-supervised-training training_demo.c)
target_link_libraries(umicom-native-supervised-training PRIVATE
    Umicom::AiSupervisedExperiment)
target_include_directories(umicom-native-supervised-training PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../../tests/ai_training")
target_compile_features(umicom-native-supervised-training PRIVATE c_std_23)
if(MSVC)
    target_compile_options(umicom-native-supervised-training PRIVATE /W4)
else()
    target_compile_options(umicom-native-supervised-training PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
if(BUILD_TESTING)
    add_test(NAME framework.ai.supervised.demo
             COMMAND umicom-native-supervised-training)
    set_tests_properties(framework.ai.supervised.demo PROPERTIES
        LABELS "framework;ai;supervised;learning" TIMEOUT 120)
endif()
