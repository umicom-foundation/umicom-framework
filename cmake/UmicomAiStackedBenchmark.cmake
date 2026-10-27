#-----------------------------------------------------------------------------
# Umicom Framework | Bounded native inference benchmarking
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_bench_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET Umicom::AiStackedIncremental)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiStackedIncremental.cmake")
endif()
if(NOT TARGET umicom_ai_stacked_benchmark)
    add_library(umicom_ai_stacked_benchmark STATIC
        "${_umicom_bench_root}/src/ai/training/stacked_inference_benchmark.c")
    add_library(Umicom::AiStackedBenchmark ALIAS umicom_ai_stacked_benchmark)
    target_compile_features(umicom_ai_stacked_benchmark PUBLIC c_std_23)
    target_include_directories(umicom_ai_stacked_benchmark PUBLIC
        "${_umicom_bench_root}/include")
    target_link_libraries(umicom_ai_stacked_benchmark PUBLIC
        Umicom::AiStackedIncremental)
    if(MSVC)
        target_compile_options(umicom_ai_stacked_benchmark PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_stacked_benchmark PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
if(BUILD_TESTING AND NOT TARGET umicom-ai-stacked-benchmark-test)
    add_executable(umicom-ai-stacked-benchmark-test
        "${_umicom_bench_root}/tests/ai_training/test_stacked_inference_benchmark.c")
    target_link_libraries(umicom-ai-stacked-benchmark-test PRIVATE
        Umicom::AiStackedBenchmark)
    add_test(NAME framework.ai.stacked_benchmark.parity_and_rejection
        COMMAND umicom-ai-stacked-benchmark-test)
    set_tests_properties(framework.ai.stacked_benchmark.parity_and_rejection
        PROPERTIES LABELS "framework;ai;benchmark;parity" TIMEOUT 120)
endif()
if(NOT TARGET umicom-native-stacked-benchmark)
    add_executable(umicom-native-stacked-benchmark
        "${_umicom_bench_root}/learning/native-ai-stacked-benchmark/demo.c")
    target_link_libraries(umicom-native-stacked-benchmark PRIVATE
        Umicom::AiStackedBenchmark)
endif()
