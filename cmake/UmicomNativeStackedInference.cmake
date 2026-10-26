#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeStackedInference.cmake
# PURPOSE: Qualify portable full-context inference and held-out scoring for
#          the established multi-layer Transformer, without copying kernels.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_stacked_inference_root
    "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

# The pre-existing stacked decoder, sampler, checkpoint and base hash
# remain the single authoritative implementations of their respective work.
if(NOT TARGET Umicom::AiStackedDecoder)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeStackedDecoder.cmake")
endif()

if(NOT TARGET umicom_ai_stacked_inference)
    add_library(umicom_ai_stacked_inference STATIC
        "${_umicom_stacked_inference_root}/src/ai/training/stacked_model_identity.c"
        "${_umicom_stacked_inference_root}/src/ai/training/stacked_inference.c"
        "${_umicom_stacked_inference_root}/src/ai/training/stacked_evaluation.c"
        "${_umicom_stacked_inference_root}/src/ai/training/stacked_model_selection.c"
        "${_umicom_stacked_inference_root}/src/ai/training/stacked_generation_evidence.c")
    add_library(Umicom::AiStackedInference ALIAS umicom_ai_stacked_inference)
    target_include_directories(umicom_ai_stacked_inference PUBLIC
        "${_umicom_stacked_inference_root}/include")
    target_link_libraries(umicom_ai_stacked_inference PUBLIC
        Umicom::AiStackedDecoder Umicom::AiTraining)
    target_compile_features(umicom_ai_stacked_inference PUBLIC c_std_23)
    # POSIX libm supplies exp() for reported perplexity; Windows uses CRT.
    if(NOT MSVC AND NOT WIN32)
        target_link_libraries(umicom_ai_stacked_inference PUBLIC m)
    endif()
    if(MSVC)
        target_compile_options(umicom_ai_stacked_inference PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_stacked_inference PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

if(BUILD_TESTING)
    function(_umicom_stacked_inference_test case_name)
        set(_name "umicom-ai-stacked-inference-${case_name}-test")
        if(TARGET "${_name}")
            return()
        endif()
        add_executable("${_name}"
            "${_umicom_stacked_inference_root}/tests/ai_training/test_stacked_inference_${case_name}.c")
        target_link_libraries("${_name}" PRIVATE Umicom::AiStackedInference)
        target_compile_features("${_name}" PRIVATE c_std_23)
        if(MSVC)
            target_compile_options("${_name}" PRIVATE /W4)
        else()
            target_compile_options("${_name}" PRIVATE
                -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
        endif()
        add_test(NAME "framework.ai.stacked_inference.${case_name}" COMMAND "${_name}")
        set_tests_properties("framework.ai.stacked_inference.${case_name}" PROPERTIES
            LABELS "framework;ai;inference;stacked" TIMEOUT 120)
    endfunction()
    foreach(_case IN ITEMS
        identity session reference rejections bytes evidence
        evaluation evaluation_rejections selection lifetime)
        _umicom_stacked_inference_test("${_case}")
    endforeach()
endif()

if(NOT TARGET umicom-native-stacked-inference-demo)
    add_executable(umicom-native-stacked-inference-demo
        "${_umicom_stacked_inference_root}/learning/native-ai-stacked-inference/demo.c")
    target_link_libraries(umicom-native-stacked-inference-demo PRIVATE
        Umicom::AiStackedInference)
    target_compile_features(umicom-native-stacked-inference-demo PRIVATE c_std_23)
endif()
