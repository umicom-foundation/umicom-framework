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

# The Batch 02 demonstration is another thin consumer of canonical Framework
# training primitives. It validates a context-sensitive educational corpus.
add_executable(umicom-native-llm-attention-demo
    "${CMAKE_CURRENT_LIST_DIR}/train_attention.c")
target_link_libraries(umicom-native-llm-attention-demo PRIVATE Umicom::AiTraining)
target_compile_features(umicom-native-llm-attention-demo PRIVATE c_std_23)
if(MSVC)
    target_compile_options(umicom-native-llm-attention-demo PRIVATE /W4)
else()
    target_compile_options(umicom-native-llm-attention-demo PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
if(BUILD_TESTING)
    add_test(NAME framework.ai.training.attention_demo
             COMMAND umicom-native-llm-attention-demo)
    set_tests_properties(framework.ai.training.attention_demo PROPERTIES
        LABELS "framework;ai;native-training;learning")
endif()

# Batch 03: native multi-head pre-LN Transformer Block training. This executable
# composes canonical Framework operators and never duplicates neural maths.
add_executable(umicom-native-llm-transformer-demo
    "${CMAKE_CURRENT_LIST_DIR}/train_transformer.c")
target_link_libraries(umicom-native-llm-transformer-demo PRIVATE Umicom::AiTraining)
target_compile_features(umicom-native-llm-transformer-demo PRIVATE c_std_23)
if(MSVC)
    target_compile_options(umicom-native-llm-transformer-demo PRIVATE /W4)
else()
    target_compile_options(umicom-native-llm-transformer-demo PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
if(BUILD_TESTING)
    add_test(NAME framework.ai.training.transformer_demo
             COMMAND umicom-native-llm-transformer-demo)
    set_tests_properties(framework.ai.training.transformer_demo PROPERTIES
        LABELS "framework;ai;native-training;learning")
endif()
