#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeLlmTraining.cmake
#
# PURPOSE:
#   Compose the additive, Framework-owned C23 automatic differentiation
#   foundation and its focused tests. No existing AI target is replaced.
#
# LIFECYCLE:
#   This module can be included from the dedicated learning CMake project
#   immediately, or from the canonical Framework CMakeLists after review.
#   The module is include-guarded to prevent duplicate registration.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

get_filename_component(_umicom_ai_training_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET umicom_ai_training)
    add_library(umicom_ai_training STATIC
        "${_umicom_ai_training_root}/src/ai/training/tape.c"
    )
    add_library(Umicom::AiTraining ALIAS umicom_ai_training)
    target_include_directories(umicom_ai_training PUBLIC
        "$<BUILD_INTERFACE:${_umicom_ai_training_root}/include>"
    )
    target_compile_features(umicom_ai_training PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_training PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_training PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow
        )
    endif()
    # libm is required by some Unix linkers for exp() and log().
    if(NOT MSVC AND NOT WIN32)
        target_link_libraries(umicom_ai_training PUBLIC m)
    endif()
endif()

if(BUILD_TESTING)
    function(_umicom_add_native_ai_training_test target test_name source_file)
        if(TARGET "${target}")
            return()
        endif()
        add_executable("${target}"
            "${_umicom_ai_training_root}/tests/ai_training/${source_file}")
        target_link_libraries("${target}" PRIVATE Umicom::AiTraining)
        target_compile_features("${target}" PRIVATE c_std_23)
        if(MSVC)
            target_compile_options("${target}" PRIVATE /W4)
        else()
            target_compile_options("${target}" PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
        add_test(NAME "${test_name}" COMMAND "${target}")
        set_tests_properties("${test_name}" PROPERTIES
            LABELS "framework;ai;native-training")
    endfunction()

    _umicom_add_native_ai_training_test(
        umicom-ai-training-contract-test
        framework.ai.training.contract
        test_training_contract.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-gradient-test
        framework.ai.training.gradients
        test_training_gradients.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-regression-test
        framework.ai.training.regression
        test_training_regression.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-bigram-test
        framework.ai.training.bigram
        test_training_bigram.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-finite-differences-test
        framework.ai.training.finite_differences
        test_training_numerical_gradients.c)
    _umicom_add_native_ai_training_test(
        umicom-ai-training-limits-test
        framework.ai.training.limits
        test_training_limits.c)
endif()
