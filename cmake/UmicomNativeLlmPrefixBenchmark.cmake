#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeLlmPrefixBenchmark.cmake
# PURPOSE:
#   Register opt-in C23 paired performance and checkpoint integrity analysis
#   on the SAME decoder, sampler and KV cache. No alternative engine.
# REUSE POLICY:
#   Include after UmicomNativeLlmTraining.cmake and
#   UmicomNativeLlmPrefixReuse.cmake. Keep this as a shared Framework module,
#   not an application-local benchmark implementation.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_benchmark_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET umicom_ai_training)
    message(FATAL_ERROR "Include UmicomNativeLlmTraining.cmake first")
endif()
if(NOT TARGET umicom-native-llm-prefix-generation)
    message(FATAL_ERROR "Include UmicomNativeLlmPrefixReuse.cmake first")
endif()

target_sources(umicom_ai_training PRIVATE
    "${_umicom_benchmark_root}/src/ai/training/prefix_benchmark.c")

if(NOT TARGET umicom-native-llm-prefix-benchmark)
    add_executable(umicom-native-llm-prefix-benchmark
        "${_umicom_benchmark_root}/learning/native-llm/prefix-work/benchmark_prefix_generation.c")
    target_link_libraries(umicom-native-llm-prefix-benchmark PRIVATE Umicom::AiTraining)
    target_compile_features(umicom-native-llm-prefix-benchmark PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-native-llm-prefix-benchmark PRIVATE /W4)
    else()
        target_compile_options(umicom-native-llm-prefix-benchmark PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

if(BUILD_TESTING)
    foreach(_umicom_benchmark_case IN ITEMS parity rejections)
        set(_umicom_benchmark_target
            "umicom-ai-training-prefix-benchmark-${_umicom_benchmark_case}-test")
        if(NOT TARGET "${_umicom_benchmark_target}")
            add_executable("${_umicom_benchmark_target}"
                "${_umicom_benchmark_root}/tests/ai_training/test_training_prefix_benchmark_${_umicom_benchmark_case}.c")
            target_link_libraries("${_umicom_benchmark_target}" PRIVATE Umicom::AiTraining)
            target_compile_features("${_umicom_benchmark_target}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${_umicom_benchmark_target}" PRIVATE /W4)
            else()
                target_compile_options("${_umicom_benchmark_target}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            endif()
            add_test(NAME "framework.ai.training.prefix_benchmark_${_umicom_benchmark_case}"
                     COMMAND "${_umicom_benchmark_target}")
            set_tests_properties(
                "framework.ai.training.prefix_benchmark_${_umicom_benchmark_case}"
                PROPERTIES LABELS "framework;ai;native-training;prefix-benchmark;checkpoint")
        endif()
    endforeach()
    add_test(NAME framework.ai.training.prefix_benchmark_demo
             COMMAND umicom-native-llm-prefix-benchmark)
    set_tests_properties(framework.ai.training.prefix_benchmark_demo
        PROPERTIES LABELS "framework;ai;native-training;prefix-benchmark;learning")
endif()
