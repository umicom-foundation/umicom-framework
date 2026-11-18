#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/training_corpus_memory/CMakeLists.txt
#
# PURPOSE:
#   Qualify corpus ownership against production source and existing import regressions.
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
    project(UmicomCorpusMemoryChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_corpus_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    add_library(umicom_corpus_subject STATIC "${_corpus_root}/src/ai/training/corpus.c" "${_corpus_root}/src/base/sha256.c")
    target_include_directories(umicom_corpus_subject PUBLIC "${_corpus_root}/include")
    target_compile_features(umicom_corpus_subject PUBLIC c_std_23)
    set(_corpus_library umicom_corpus_subject)
else()
    set(_corpus_library Umicom::AiTraining)
endif()
add_executable(umicom-corpus-memory-test test_corpus_memory.c)
target_link_libraries(umicom-corpus-memory-test PRIVATE ${_corpus_library})
target_compile_features(umicom-corpus-memory-test PRIVATE c_std_23)
if(NOT MSVC)
    target_compile_options(umicom-corpus-memory-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-corpus-memory-test)
endif()
foreach(_case IN ITEMS maximum allocations bounds independence)
    add_test(NAME "framework.ai.training.corpus_memory.${_case}" COMMAND umicom-corpus-memory-test "${_case}" "${CMAKE_CURRENT_BINARY_DIR}/fixture-${_case}")
    set_tests_properties("framework.ai.training.corpus_memory.${_case}" PROPERTIES TIMEOUT 30 LABELS "framework;ai;training;memory;corpus")
endforeach()
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    foreach(_case IN ITEMS corpus corpus_file corpus_ranges corpus_source_sha256)
        add_executable(corpus-regression-${_case} "../ai_training/test_training_${_case}.c")
        target_link_libraries(corpus-regression-${_case} PRIVATE ${_corpus_library})
        add_test(NAME "framework.ai.training.corpus_memory.regression.${_case}" COMMAND corpus-regression-${_case})
        set_tests_properties("framework.ai.training.corpus_memory.regression.${_case}" PROPERTIES TIMEOUT 30)
    endforeach()
endif()
