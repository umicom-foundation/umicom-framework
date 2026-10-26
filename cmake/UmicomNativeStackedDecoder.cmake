#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeStackedDecoder.cmake
# PURPOSE: Compose real multi-layer Transformer training and explicit snapshot
#          ownership from existing Framework native AI mathematics.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_stacked_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

# Reuse existing Framework authorities: the tape and Transformer compose
# gradients, supervised loss selects targets, and scaling owns the maths used
# to estimate model capacity. No application or independent runtime owns them.
if(NOT TARGET Umicom::AiTraining)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeLlmTraining.cmake")
endif()
if(NOT TARGET Umicom::AiAdaptation)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiAdaptation.cmake")
endif()
if(NOT TARGET Umicom::AiScaling)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiScaling.cmake")
endif()

if(NOT TARGET umicom_ai_stacked_decoder)
    add_library(umicom_ai_stacked_decoder STATIC
        "${_umicom_stacked_root}/src/ai/training/stacked_decoder.c"
        "${_umicom_stacked_root}/src/ai/training/stacked_checkpoint.c"
        "${_umicom_stacked_root}/src/ai/training/stacked_experiment.c")
    add_library(Umicom::AiStackedDecoder ALIAS umicom_ai_stacked_decoder)
    target_include_directories(umicom_ai_stacked_decoder PUBLIC
        "${_umicom_stacked_root}/include")
    target_link_libraries(umicom_ai_stacked_decoder PUBLIC
        Umicom::AiTraining Umicom::AiAdaptation Umicom::AiScaling)
    target_compile_features(umicom_ai_stacked_decoder PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_stacked_decoder PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_stacked_decoder PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

if(BUILD_TESTING)
    function(_umicom_stacked_test suffix)
        set(_target "umicom-ai-stacked-${suffix}-test")
        if(TARGET "${_target}")
            return()
        endif()
        add_executable("${_target}"
            "${_umicom_stacked_root}/tests/ai_training/test_stacked_${suffix}.c")
        target_link_libraries("${_target}" PRIVATE Umicom::AiStackedDecoder)
        target_compile_features("${_target}" PRIVATE c_std_23)
        if(MSVC)
            target_compile_options("${_target}" PRIVATE /W4)
        else()
            target_compile_options("${_target}" PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
        add_test(NAME "framework.ai.stacked.${suffix}" COMMAND "${_target}")
        set_tests_properties("framework.ai.stacked.${suffix}" PROPERTIES
            LABELS "framework;ai;native-training;stacked" TIMEOUT 90)
    endfunction()
    foreach(_case IN ITEMS
        configuration forward causality training rejections generation
        checkpoint checkpoint_rejections resume
        experiment experiment_rejections memory)
        _umicom_stacked_test("${_case}")
    endforeach()
endif()

if(NOT TARGET umicom-native-stacked-demo)
    add_executable(umicom-native-stacked-demo
        "${_umicom_stacked_root}/learning/native-ai-stacked/demo.c")
    target_link_libraries(umicom-native-stacked-demo PRIVATE Umicom::AiStackedDecoder)
    target_compile_features(umicom-native-stacked-demo PRIVATE c_std_23)
endif()
