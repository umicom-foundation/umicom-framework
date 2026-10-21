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

# Batch 04: a thin learner-facing executable that trains/evaluates a complete
# bounded decoder, writes a reviewed NEW checkpoint and reloads it. All maths,
# format validation and file publication remain Framework-owned.
add_executable(umicom-native-llm-decoder-demo
    "${CMAKE_CURRENT_LIST_DIR}/train_decoder.c")
target_link_libraries(umicom-native-llm-decoder-demo PRIVATE Umicom::AiTraining)
target_compile_features(umicom-native-llm-decoder-demo PRIVATE c_std_23)
if(MSVC)
    target_compile_options(umicom-native-llm-decoder-demo PRIVATE /W4)
else()
    target_compile_options(umicom-native-llm-decoder-demo PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
if(BUILD_TESTING)
    add_test(NAME framework.ai.training.decoder_demo
             COMMAND umicom-native-llm-decoder-demo)
    set_tests_properties(framework.ai.training.decoder_demo PROPERTIES
        LABELS "framework;ai;native-training;learning")
endif()

# Batch 05: thin read-only source-corpus inspector, with native Framework owners.
add_executable(umicom-native-llm-corpus
    "${CMAKE_CURRENT_LIST_DIR}/inspect_corpus.c")
target_link_libraries(umicom-native-llm-corpus PRIVATE Umicom::AiTraining)
target_compile_features(umicom-native-llm-corpus PRIVATE c_std_23)
if(MSVC)
    target_compile_options(umicom-native-llm-corpus PRIVATE /W4)
else()
    target_compile_options(umicom-native-llm-corpus PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

# Batch 06: train the canonical decoder on source-code bytes selected by a
# reviewed file-level manifest. This tool is a thin consumer; no training
# algorithms, tokenisation or persistence live outside the Framework.
add_executable(umicom-native-llm-source-train
    "${CMAKE_CURRENT_LIST_DIR}/train_source.c")
target_link_libraries(umicom-native-llm-source-train PRIVATE Umicom::AiTraining)
target_compile_features(umicom-native-llm-source-train PRIVATE c_std_23)
if(MSVC)
    target_compile_options(umicom-native-llm-source-train PRIVATE /W4)
else()
    target_compile_options(umicom-native-llm-source-train PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
# No CLI training is executed automatically by CTest: the focused source
# model regression exercises identical functionality in a bounded test.
