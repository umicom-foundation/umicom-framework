#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiInferenceMemoryPlan.cmake
#
# PURPOSE:
#   Register the portable C23 K/V memory planner and its unit tests.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_memory_plan_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET umicom_ai_inference_memory_plan)
    add_library(umicom_ai_inference_memory_plan STATIC
        "${_umicom_memory_plan_root}/src/ai/training/inference_memory_plan.c")
    add_library(Umicom::AiInferenceMemoryPlan ALIAS umicom_ai_inference_memory_plan)
    target_include_directories(umicom_ai_inference_memory_plan PUBLIC
        "${_umicom_memory_plan_root}/include")
    target_compile_features(umicom_ai_inference_memory_plan PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_inference_memory_plan PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_inference_memory_plan PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
if(BUILD_TESTING AND NOT TARGET umicom_ai_inference_memory_plan_test)
    add_executable(umicom_ai_inference_memory_plan_test
        "${_umicom_memory_plan_root}/tests/ai_training/test_inference_memory_plan.c")
    target_link_libraries(umicom_ai_inference_memory_plan_test PRIVATE
        Umicom::AiInferenceMemoryPlan)
    add_test(NAME framework.ai.inference_memory_plan
        COMMAND umicom_ai_inference_memory_plan_test)
endif()
