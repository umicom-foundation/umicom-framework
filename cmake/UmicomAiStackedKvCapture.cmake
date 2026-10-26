#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiStackedKvCapture.cmake
# PURPOSE: Observe canonical stacked Transformer K/V and populate the existing
#          verified storage owner without an alternative attention kernel.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(UMICOM_AI_KV_CAPTURE_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET Umicom::AiStackedInference)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeStackedInference.cmake")
endif()
if(NOT TARGET Umicom::AiStackedKvStore)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiStackedKvStore.cmake")
endif()

if(NOT TARGET umicom_ai_stacked_kv_capture)
    add_library(umicom_ai_stacked_kv_capture STATIC
        "${UMICOM_AI_KV_CAPTURE_ROOT}/src/ai/training/stacked_kv_capture.c")
    add_library(Umicom::AiStackedKvCapture ALIAS umicom_ai_stacked_kv_capture)
    target_include_directories(umicom_ai_stacked_kv_capture PUBLIC
        "${UMICOM_AI_KV_CAPTURE_ROOT}/include")
    target_link_libraries(umicom_ai_stacked_kv_capture PUBLIC
        Umicom::AiStackedInference Umicom::AiStackedKvStore)
    target_compile_features(umicom_ai_stacked_kv_capture PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_stacked_kv_capture PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_stacked_kv_capture PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()

if(BUILD_TESTING)
    foreach(test_name IN ITEMS parity rejections reset)
        set(target "umicom-ai-stacked-kv-capture-${test_name}-test")
        if(NOT TARGET "${target}")
            add_executable("${target}"
                "${UMICOM_AI_KV_CAPTURE_ROOT}/tests/ai_training/test_stacked_kv_capture_${test_name}.c")
            target_link_libraries("${target}" PRIVATE Umicom::AiStackedKvCapture)
            target_compile_features("${target}" PRIVATE c_std_23)
            add_test(NAME "framework.ai.stacked_kv_capture.${test_name}"
                     COMMAND "${target}")
            set_tests_properties("framework.ai.stacked_kv_capture.${test_name}"
                PROPERTIES LABELS "framework;ai;native-training;kv" TIMEOUT 120)
        endif()
    endforeach()
endif()

if(NOT TARGET umicom-native-stacked-kv-capture-demo)
    add_executable(umicom-native-stacked-kv-capture-demo
        "${UMICOM_AI_KV_CAPTURE_ROOT}/learning/native-ai-stacked-kv-capture/demo.c")
    target_link_libraries(umicom-native-stacked-kv-capture-demo PRIVATE
        Umicom::AiStackedKvCapture)
    target_compile_features(umicom-native-stacked-kv-capture-demo PRIVATE c_std_23)
endif()
