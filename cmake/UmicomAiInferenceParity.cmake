#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiInferenceParity.cmake
# PURPOSE: Opt-in, native C23 FP32 inference comparison and focused regression.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_parity_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET umicom_ai_inference_parity)
    add_library(umicom_ai_inference_parity STATIC
        "${_umicom_parity_root}/src/ai/training/inference_parity.c")
    add_library(Umicom::AiInferenceParity ALIAS umicom_ai_inference_parity)
    target_include_directories(umicom_ai_inference_parity PUBLIC
        "${_umicom_parity_root}/include")
    target_compile_features(umicom_ai_inference_parity PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_inference_parity PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_inference_parity PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
        target_link_libraries(umicom_ai_inference_parity PUBLIC m)
    endif()
endif()
if(BUILD_TESTING AND NOT TARGET umicom-ai-inference-parity-test)
    add_executable(umicom-ai-inference-parity-test
        "${_umicom_parity_root}/tests/ai_training/test_inference_parity.c")
    target_link_libraries(umicom-ai-inference-parity-test PRIVATE
        Umicom::AiInferenceParity)
    add_test(NAME framework.ai.inference_parity.compare
        COMMAND umicom-ai-inference-parity-test)
    set_tests_properties(framework.ai.inference_parity.compare PROPERTIES
        LABELS "framework;ai;native;parity" TIMEOUT 30)
endif()
