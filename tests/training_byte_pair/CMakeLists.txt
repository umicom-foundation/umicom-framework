#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/training_byte_pair/CMakeLists.txt
#
# PURPOSE:
#   Qualify bounded prompt/completion preparation through the canonical byte tokenizer.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomBytePairChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_pair_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    add_library(umicom_training_byte_pair_subject STATIC "${_pair_root}/src/ai/training/byte_pair_sequence.c" "${_pair_root}/src/ai/training/byte_pair_shape.c"
        "${_pair_root}/src/ai/training/byte_tokenizer.c")
    target_include_directories(umicom_training_byte_pair_subject PUBLIC "${_pair_root}/include")
    target_compile_features(umicom_training_byte_pair_subject PUBLIC c_std_23)
    if(NOT MSVC)
        target_compile_options(umicom_training_byte_pair_subject PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    set(_pair_library umicom_training_byte_pair_subject)
else()
    set(_pair_library Umicom::AiTraining)
endif()
add_executable(umicom-byte-pair-test test_byte_pair.c)
 target_link_libraries(umicom-byte-pair-test PRIVATE ${_pair_library})
 target_compile_features(umicom-byte-pair-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-byte-pair-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(NOT WIN32)
    target_link_libraries(umicom-byte-pair-test PRIVATE m)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-byte-pair-test)
endif()
foreach(_case IN ITEMS golden empty binary padding limits nulls refusal ownership grid vocabulary)
    add_test(NAME framework.ai.training.byte_pair.${_case} COMMAND umicom-byte-pair-test ${_case})
    set_tests_properties(framework.ai.training.byte_pair.${_case} PROPERTIES TIMEOUT 30 LABELS "framework;training;tokenizer;selection;ownership")
endforeach()
