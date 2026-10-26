#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeStackedPrefixInference.cmake
# PURPOSE: Compose a qualified growing-prefix inference policy from existing
#          native stacked decoder, nucleus sampling, identity and tokenisation.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_prefix_inference_root
    "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET Umicom::AiStackedInference)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeStackedInference.cmake")
endif()
if(NOT TARGET Umicom::AiStackedPrefixCachePlan)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiStackedPrefixCachePlan.cmake")
endif()

if(NOT TARGET umicom_ai_stacked_prefix_inference)
    add_library(umicom_ai_stacked_prefix_inference STATIC
        "${_umicom_prefix_inference_root}/src/ai/training/stacked_prefix_inference.c"
        "${_umicom_prefix_inference_root}/src/ai/training/stacked_prefix_qualification.c")
    add_library(Umicom::AiStackedPrefixInference
        ALIAS umicom_ai_stacked_prefix_inference)
    target_include_directories(umicom_ai_stacked_prefix_inference PUBLIC
        "${_umicom_prefix_inference_root}/include")
    target_link_libraries(umicom_ai_stacked_prefix_inference PUBLIC
        Umicom::AiStackedInference Umicom::AiStackedPrefixCachePlan)
    target_compile_features(umicom_ai_stacked_prefix_inference PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_stacked_prefix_inference PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_stacked_prefix_inference PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

if(BUILD_TESTING)
    foreach(_case IN ITEMS streaming causality boundaries)
        set(_name "umicom-ai-stacked-prefix-${_case}-test")
        if(NOT TARGET "${_name}")
            add_executable("${_name}"
                "${_umicom_prefix_inference_root}/tests/ai_training/test_stacked_prefix_${_case}.c")
            target_link_libraries("${_name}" PRIVATE
                Umicom::AiStackedPrefixInference)
            target_compile_features("${_name}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${_name}" PRIVATE /W4)
            else()
                target_compile_options("${_name}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
            endif()
            add_test(NAME "framework.ai.stacked_prefix.${_case}" COMMAND "${_name}")
            set_tests_properties("framework.ai.stacked_prefix.${_case}" PROPERTIES
                TIMEOUT 120 LABELS "framework;ai;stacked;inference;qualification")
        endif()
    endforeach()
endif()

if(NOT TARGET umicom-native-stacked-prefix-demo)
    add_executable(umicom-native-stacked-prefix-demo
        "${_umicom_prefix_inference_root}/learning/native-ai-stacked-prefix/demo.c")
    target_link_libraries(umicom-native-stacked-prefix-demo PRIVATE
        Umicom::AiStackedPrefixInference)
    target_compile_features(umicom-native-stacked-prefix-demo PRIVATE c_std_23)
endif()
